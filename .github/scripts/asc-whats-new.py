#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
"""Sets "What to Test" of an uploaded TestFlight build through the App Store Connect API.

    asc-whats-new.py <build number> <text file>

Environment: ASC_KEY_ID, ASC_ISSUER_ID, ASC_KEY_FILE (the .p8 file), ASC_APP_ID.
Waits until Apple lists the build (it appears while it is being processed), then
writes the text into every localization of the build, German (de-DE) at least.
Only the standard library and the openssl command: no packages to install.
"""
import base64
import json
import os
import subprocess
import sys
import time
import urllib.error
import urllib.parse
import urllib.request

API = "https://api.appstoreconnect.apple.com/v1"


def b64url(data: bytes) -> str:
    return base64.urlsafe_b64encode(data).rstrip(b"=").decode()


def der_to_raw(der: bytes) -> bytes:
    """ECDSA signature from openssl (DER: SEQUENCE of r and s) as JWT wants it (r || s, 32 bytes each)."""
    if der[0] != 0x30:
        raise ValueError("not a DER sequence")
    i = 2 if der[1] < 0x80 else 2 + (der[1] & 0x7F)
    parts = []
    for _ in range(2):
        if der[i] != 0x02:
            raise ValueError("not a DER integer")
        length = der[i + 1]
        value = der[i + 2:i + 2 + length].lstrip(b"\x00")
        parts.append(value.rjust(32, b"\x00"))
        i += 2 + length
    return parts[0] + parts[1]


def token(key_id: str, issuer: str, key_file: str) -> str:
    header = {"alg": "ES256", "kid": key_id, "typ": "JWT"}
    now = int(time.time())
    payload = {"iss": issuer, "iat": now, "exp": now + 15 * 60, "aud": "appstoreconnect-v1"}
    signing_input = b64url(json.dumps(header, separators=(",", ":")).encode()) + "." + \
        b64url(json.dumps(payload, separators=(",", ":")).encode())
    der = subprocess.run(["openssl", "dgst", "-sha256", "-sign", key_file], input=signing_input.encode(),
                         capture_output=True, check=True).stdout
    return signing_input + "." + b64url(der_to_raw(der))


class Api:
    def __init__(self):
        self.key_id = os.environ["ASC_KEY_ID"]
        self.issuer = os.environ["ASC_ISSUER_ID"]
        self.key_file = os.environ["ASC_KEY_FILE"]

    def call(self, method: str, path: str, body=None):
        data = json.dumps(body).encode() if body is not None else None
        request = urllib.request.Request(API + path, data=data, method=method)
        request.add_header("Authorization", "Bearer " + token(self.key_id, self.issuer, self.key_file))
        if data is not None:
            request.add_header("Content-Type", "application/json")
        try:
            with urllib.request.urlopen(request, timeout=60) as response:
                text = response.read().decode()
                return json.loads(text) if text else {}
        except urllib.error.HTTPError as error:
            raise RuntimeError(f"{method} {path}: HTTP {error.code} {error.read().decode()[:600]}") from None


def main() -> int:
    build_number, text_file = sys.argv[1], sys.argv[2]
    text = open(text_file, encoding="utf-8").read().strip()
    if not text:
        print("No text given: nothing to set.")
        return 0
    if len(text) > 4000:
        print(f"::warning::What to Test is limited to 4000 characters, the text has {len(text)}; it is cut.")
        text = text[:4000]
    api = Api()
    app = os.environ["ASC_APP_ID"]
    query = urllib.parse.urlencode({"filter[app]": app, "filter[version]": build_number, "limit": 5})

    # The build shows up a few minutes after the upload, while Apple processes it
    build = None
    deadline = time.time() + 50 * 60
    while time.time() < deadline:
        found = api.call("GET", "/builds?" + query).get("data", [])
        if found:
            build = found[0]
            break
        print(f"Build {build_number} not listed yet, waiting …", flush=True)
        time.sleep(60)
    if build is None:
        print(f"::warning::Build {build_number} did not show up in App Store Connect within 50 minutes; "
              "What to Test was not set.")
        return 1
    build_id = build["id"]
    print(f"Build {build_number}: {build_id} ({build['attributes'].get('processingState')})")

    localizations = api.call("GET", f"/builds/{build_id}/betaBuildLocalizations").get("data", [])
    locales = {item["attributes"]["locale"]: item["id"] for item in localizations}
    for locale, loc_id in locales.items():
        api.call("PATCH", f"/betaBuildLocalizations/{loc_id}",
                 {"data": {"type": "betaBuildLocalizations", "id": loc_id, "attributes": {"whatsNew": text}}})
        print(f"What to Test set ({locale}).")
    if "de-DE" not in locales:
        api.call("POST", "/betaBuildLocalizations",
                 {"data": {"type": "betaBuildLocalizations",
                           "attributes": {"locale": "de-DE", "whatsNew": text},
                           "relationships": {"build": {"data": {"type": "builds", "id": build_id}}}}})
        print("What to Test set (de-DE, new).")
    return 0


if __name__ == "__main__":
    sys.exit(main())
