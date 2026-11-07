#!/usr/bin/env python3
"""Collected TestFlight builds (run by .github/workflows/testflight-collect.yml).

Builds are not started after every push but at about 13:05 and 22:05 German time, with
everything collected since the last upload, so testers are not notified for each small change.

At each slot:
  - a TestFlight run that is still going: nothing to do
  - otherwise a build is started only if something that ends up in the iOS app changed since
    the last build that really reached TestFlight (upload step succeeded); checked once more
    right before starting, with .github/testflight/whats-new.txt as test text
then wait for that run (re-run once if no macOS runner was free) and write the result to the
branch "testflight-status" (status.json, status.md). That branch can be read with a plain
git clone, without signing in.

Only the Python standard library. Environment: GITHUB_TOKEN, GITHUB_REPOSITORY,
COLLECT_MODE (schedule | dry-run | now), CRON (the schedule that fired), WHATS_NEW_FILE.
Locally:  COLLECT_MODE=dry-run NO_STATUS=1 python3 .github/scripts/testflight-collect.py
"""

import base64
import datetime as dt
import json
import os
import re
import sys
import time
import urllib.error
import urllib.request
from zoneinfo import ZoneInfo

REPO = os.environ.get("GITHUB_REPOSITORY", "doitux/qupil2")
TOKEN = os.environ.get("GITHUB_TOKEN") or os.environ.get("GH_TOKEN", "")
MODE = os.environ.get("COLLECT_MODE", "schedule")
CRON = os.environ.get("CRON", "")
WHATS_NEW_FILE = os.environ.get("WHATS_NEW_FILE", ".github/testflight/whats-new.txt")
NO_STATUS = os.environ.get("NO_STATUS") == "1"

WORKFLOW = "testflight.yml"
STATUS_BRANCH = "testflight-status"
BERLIN = ZoneInfo("Europe/Berlin")
UPLOAD_STEP = "Sign and upload to TestFlight"
WHATS_NEW_STEP = 'Set "What to Test"'
WAIT_MINUTES = 110

# Slots in German time and the latest start for each (later: the next slot takes it).
# GitHub starts scheduled runs late, sometimes by hours. Midday: only on time, so the
# build is at TestFlight before the afternoon. Evening: until 06:30 the next morning
# (the night is free; a build takes about half an hour, Apple's processing comes on top).
SLOTS = {(13, 5): (13, 10), (22, 5): (6, 30)}
# The midday slot only Friday to Sunday: Monday to Thursday a build at 13:05 would reach
# the testers (after Apple's processing) in the afternoon's working hours
MIDDAY_DAYS = {4, 5, 6}   # Monday = 0

# Changes that do not end up in the iOS app
IGNORE = [re.compile(p) for p in (
    r"^platform/(windows|linux|macos|android)/",
    r"_android\.(cpp|h|java)$",
    r"^\.github/workflows/(desktop|mobile|translations|testflight-collect)\.yml$",
    r"^\.github/scripts/testflight-collect\.py$",
    r"^\.github/testflight/",
    r"^\.github/dependabot\.yml$",
    r"^docs/",
    r"\.md$",
)]


def log(*args):
    print(*args, flush=True)


def api(method, path, body=None, ok=(200, 201, 204)):
    url = path if path.startswith("https://") else f"https://api.github.com{path}"
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(url, data=data, method=method)
    req.add_header("Accept", "application/vnd.github+json")
    req.add_header("X-GitHub-Api-Version", "2022-11-28")
    req.add_header("User-Agent", "qupil-testflight-collect")
    if TOKEN:
        req.add_header("Authorization", f"Bearer {TOKEN}")
    if data is not None:
        req.add_header("Content-Type", "application/json")
    for attempt in range(4):
        try:
            with urllib.request.urlopen(req, timeout=60) as resp:
                raw = resp.read()
                return json.loads(raw) if raw else None
        except urllib.error.HTTPError as e:
            if e.code in (502, 503, 504) and attempt < 3:
                time.sleep(5 * (attempt + 1))
                continue
            if e.code == 404 and 404 in ok:
                return None
            raise RuntimeError(f"{method} {path}: HTTP {e.code} {e.read()[:300]!r}") from None


def now():
    return dt.datetime.now(BERLIN)


# --------------------------------------------------------------------------- slot


def slot_for_schedule():
    """Which slot this scheduled run stands for, or None (the cron of the other season).

    GitHub's cron is UTC, so each slot has a winter and a summer cron; only the one that
    falls on the slot in German time today counts. A delayed run is still that slot."""
    m = re.match(r"^\s*(\d+)\s+(\d+)\s", CRON)
    if not m:
        return None
    t = now()
    utc = t.astimezone(dt.timezone.utc)
    intended = utc.replace(hour=int(m.group(2)), minute=int(m.group(1)), second=0, microsecond=0)
    if intended > utc:                      # delayed past midnight UTC
        intended -= dt.timedelta(days=1)
    local = intended.astimezone(BERLIN)
    if (local.hour, local.minute) not in SLOTS:
        return None
    return local


def deadline(slot):
    h, m = SLOTS[(slot.hour, slot.minute)]
    end = slot.replace(hour=h, minute=m, second=59)
    if end < slot:                          # the latest start is on the next morning
        end += dt.timedelta(days=1)
    return end


# --------------------------------------------------------------------------- runs


def dispatch_runs():
    runs = api("GET", f"/repos/{REPO}/actions/workflows/{WORKFLOW}/runs"
                      f"?event=workflow_dispatch&per_page=30")["workflow_runs"]
    return sorted(runs, key=lambda r: r["run_number"], reverse=True)


def jobs(run_id):
    return api("GET", f"/repos/{REPO}/actions/runs/{run_id}/jobs?filter=latest")["jobs"]


def step(run_id, name):
    for job in jobs(run_id):
        for s in job.get("steps") or []:
            if s["name"] == name:
                return s.get("conclusion") or s.get("status")
    return None


def not_acquired(run):
    """The run failed because GitHub had no macOS runner free (the code is not to blame)."""
    if run.get("status") != "completed" or run.get("conclusion") != "failure":
        return False
    for job in jobs(run["id"]):
        if job.get("conclusion") == "failure" and not job.get("steps"):
            return True
        try:
            notes = api("GET", f"/repos/{REPO}/check-runs/{job['id']}/annotations") or []
        except RuntimeError:
            notes = []
        if any("not acquired by Runner" in (n.get("message") or "") for n in notes):
            return True
    return False


def uploaded(run):
    return run.get("conclusion") == "success" and step(run["id"], UPLOAD_STEP) == "success"


def describe(run):
    return {
        "number": run["run_number"],
        "run_id": run["id"],
        "sha": run["head_sha"],
        "url": run["html_url"],
        "created": run["created_at"],
        "status": run["status"],
        "conclusion": run.get("conclusion"),
        "attempt": run.get("run_attempt", 1),
    }


def wait_for(run_id, rerun_allowed=True):
    """Wait until the run is done; re-run once if no runner was free. Returns (run, rerun)."""
    rerun = False
    end = time.time() + WAIT_MINUTES * 60
    while time.time() < end:
        run = api("GET", f"/repos/{REPO}/actions/runs/{run_id}")
        if run["status"] == "completed":
            if rerun_allowed and not rerun and not_acquired(run):
                log(f"Run {run['run_number']}: no macOS runner free, running it again")
                api("POST", f"/repos/{REPO}/actions/runs/{run_id}/rerun")
                rerun = True
                time.sleep(30)
                continue
            return run, rerun
        time.sleep(60)
    return api("GET", f"/repos/{REPO}/actions/runs/{run_id}"), rerun


# --------------------------------------------------------------------------- changes


def changes_since(base):
    cmp = api("GET", f"/repos/{REPO}/compare/{base}...main")
    files = [f["filename"] for f in cmp.get("files") or []]
    relevant = [f for f in files if not any(p.search(f) for p in IGNORE)]
    # The API lists at most 300 files: with more, count it as relevant
    if len(files) >= 300:
        relevant = relevant or files
    commits = [{"sha": c["sha"][:7], "subject": c["commit"]["message"].splitlines()[0]}
               for c in cmp.get("commits") or []]
    return cmp, relevant, commits


# --------------------------------------------------------------------------- status


RESULT_DE = {
    "uploaded": "Build {n} hochgeladen",
    "upload_failed": "Build {n} fehlgeschlagen",
    "not_finished": "Build {n} nach {w} Minuten noch nicht fertig",
    "started": "Build {n} gestartet",
    "running": "Build {n} lief noch, nichts gestartet",
    "no_change": "Kein Build nötig",
    "late": "Verspätet gestartet, kein Build",
    "weekday_skip": "Mittags Mo–Do kein Build",
    "would_build": "Probelauf: würde bauen",
    "error": "Fehler im Sammel-Lauf",
}


def write_status(status):
    status["written"] = now().isoformat(timespec="seconds")
    n = (status.get("build") or {}).get("number", "?")
    headline = RESULT_DE[status["result"]].format(n=n, w=WAIT_MINUTES)
    status["headline"] = headline
    md = [f"# TestFlight-Sammellauf {status.get('slot', '')}", "", f"**{headline}**", ""]
    if status.get("note"):
        md += [status["note"], ""]
    b = status.get("build")
    if b:
        md += [f"- Build {b['number']} (Commit {b['sha'][:7]}): {b.get('conclusion') or b['status']}"
               f", Upload: {b.get('upload') or '-'}, Testtext: {b.get('whats_new') or '-'}"
               + (", per Re-run neu gestartet" if b.get("rerun") else ""),
               f"- {b['url']}"]
    if status.get("since"):
        md.append(f"- Seit Build {status.get('since_build')} (Commit {status['since'][:7]})")
    if status.get("commits"):
        md += ["", "Commits:"] + [f"- {c['sha']} {c['subject']}" for c in status["commits"]]
    if status.get("relevant_files"):
        md += ["", "Für iOS relevante Dateien:"] + [f"- {f}" for f in status["relevant_files"]]
    md_text = "\n".join(md) + "\n"
    log(md_text)
    log(json.dumps(status, indent=2, ensure_ascii=False))
    if NO_STATUS:
        return

    files = {"status.json": json.dumps(status, indent=2, ensure_ascii=False) + "\n",
             "status.md": md_text}
    ref = api("GET", f"/repos/{REPO}/git/ref/heads/{STATUS_BRANCH}", ok=(200, 404))
    parent = ref["object"]["sha"] if ref else None
    tree = api("POST", f"/repos/{REPO}/git/trees", {"tree": [
        {"path": p, "mode": "100644", "type": "blob",
         "sha": api("POST", f"/repos/{REPO}/git/blobs",
                    {"content": base64.b64encode(c.encode()).decode(), "encoding": "base64"})["sha"]}
        for p, c in files.items()]})
    commit = api("POST", f"/repos/{REPO}/git/commits", {
        "message": f"{status.get('slot', '')}: {headline} [skip ci]",
        "tree": tree["sha"], "parents": [parent] if parent else []})
    if parent:
        api("PATCH", f"/repos/{REPO}/git/refs/heads/{STATUS_BRANCH}", {"sha": commit["sha"], "force": True})
    else:
        api("POST", f"/repos/{REPO}/git/refs", {"ref": f"refs/heads/{STATUS_BRANCH}", "sha": commit["sha"]})


# --------------------------------------------------------------------------- main


def main():
    t = now()
    status = {"mode": MODE, "started_at": t.isoformat(timespec="seconds")}

    if MODE == "schedule":
        slot = slot_for_schedule()
        if slot is None:
            log(f"Cron {CRON!r} is not a slot in German time today ({t:%H:%M}); nothing to do")
            return
        status["slot"] = f"{slot:%Y-%m-%d %H:%M}"
        if (slot.hour, slot.minute) == (13, 5) and slot.weekday() not in MIDDAY_DAYS:
            status["result"] = "weekday_skip"
            status["note"] = "Montag bis Donnerstag kein Mittags-Build; die Änderungen kommen um 22:05."
            write_status(status)
            return
        if t > deadline(slot):
            status["result"] = "late"
            status["note"] = (f"GitHub hat den Lauf erst um {t:%H:%M} gestartet (geplant {slot:%H:%M}); "
                              "die Änderungen kommen in den nächsten Sammel-Lauf.")
            write_status(status)
            return
    else:
        status["slot"] = f"{t:%Y-%m-%d %H:%M} (von Hand, {MODE})"

    runs = dispatch_runs()
    active = [r for r in runs if r["status"] != "completed"]
    if active:
        run = active[0]
        status["result"] = "running"
        status["build"] = describe(run)
        write_status(status)
        return

    # Compared with the last build that really reached TestFlight (upload step succeeded);
    # failed, cancelled or check-only runs do not count. Old failed runs are not run again:
    # if their changes are still missing, the new build below contains them.
    base = next((r for r in runs if uploaded(r)), None)
    if base is None:
        raise RuntimeError("no successful TestFlight upload found to compare with")
    cmp, relevant, commits = changes_since(base["head_sha"])
    if cmp.get("status") not in ("ahead",) or not cmp.get("commits"):
        relevant = []          # main is the uploaded commit (or not after it): nothing new
    status["since"] = base["head_sha"]
    status["since_build"] = base["run_number"]
    status["main"] = cmp["commits"][-1]["sha"] if cmp.get("commits") else base["head_sha"]
    status["commits"] = commits
    status["relevant_files"] = relevant[:50]

    if not relevant:
        status["result"] = "no_change"
        status["note"] = ("Seit dem letzten Upload nichts Neues." if not commits else
                          "Seit dem letzten Upload nur Änderungen für andere Plattformen oder Begleitmaterial.")
        write_status(status)
        return

    if MODE == "dry-run":
        status["result"] = "would_build"
        status["note"] = f"{len(relevant)} Dateien für iOS geändert; im Probelauf wird nichts gestartet."
        write_status(status)
        return

    # Once more right before starting: nothing running, nothing newer uploaded meanwhile
    runs = dispatch_runs()
    if any(r["status"] != "completed" for r in runs) or \
            next((r for r in runs if uploaded(r)), base)["id"] != base["id"]:
        raise RuntimeError("another TestFlight run appeared meanwhile; nothing started")

    with open(WHATS_NEW_FILE, encoding="utf-8") as f:
        whats_new = f.read().strip()
    started = dt.datetime.now(dt.timezone.utc) - dt.timedelta(seconds=5)
    api("POST", f"/repos/{REPO}/actions/workflows/{WORKFLOW}/dispatches",
        {"ref": "main", "inputs": {"whats_new": whats_new}})
    run = None
    for _ in range(30):
        time.sleep(10)
        for r in dispatch_runs():
            if dt.datetime.fromisoformat(r["created_at"].replace("Z", "+00:00")) >= started:
                run = r
                break
        if run:
            break
    if run is None:
        raise RuntimeError("TestFlight run was dispatched but did not show up")
    log(f"Started build {run['run_number']} ({run['html_url']})")
    status["result"] = "started"
    status["build"] = describe(run)
    write_status(status)
    finish(status, run["id"])


def finish(status, run_id, rerun_done=False):
    run, rerun = wait_for(run_id, rerun_allowed=not rerun_done)
    b = describe(run)
    b["rerun"] = rerun or rerun_done
    b["upload"] = step(run_id, UPLOAD_STEP)
    b["whats_new"] = step(run_id, WHATS_NEW_STEP)
    status["build"] = b
    if run["status"] != "completed":
        status["result"] = "not_finished"
    else:
        status["result"] = "uploaded" if b["upload"] == "success" else "upload_failed"
    write_status(status)
    if status["result"] == "upload_failed":
        sys.exit(1)


if __name__ == "__main__":
    try:
        main()
    except Exception as e:  # report the failure on the status branch too
        log(f"Error: {e}")
        try:
            write_status({"mode": MODE, "slot": f"{now():%Y-%m-%d %H:%M}", "result": "error",
                          "note": str(e)[:500]})
        except Exception as e2:
            log(f"Could not write the status: {e2}")
        sys.exit(1)
