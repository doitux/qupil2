#!/usr/bin/env python3
"""The essentials of a crash report (.ips) of the simulator: why, and where the faulting
thread was.

    ios-crash-summary.py <report.ips>"""
import json
import sys


def main():
    with open(sys.argv[1], encoding='utf-8', errors='replace') as f:
        header, _, body = f.read().partition('\n')
    try:
        report = json.loads(body)
    except ValueError:
        print(header[:400])
        return 0
    print('captured:', report.get('captureTime', '?'), ' launched:', report.get('procLaunch', '?'))
    print('exception:', json.dumps(report.get('exception', {})))
    print('termination:', json.dumps(report.get('termination', {})))
    for key in ('asi', 'lastExceptionBacktrace'):
        if key == 'asi' and report.get(key):
            print('says:', json.dumps(report[key])[:600])
    images = report.get('usedImages', [])
    threads = report.get('threads', [])
    faulting = report.get('faultingThread', 0)
    frames = report.get('lastExceptionBacktrace') or (threads[faulting].get('frames', []) if faulting < len(threads) else [])
    print('thread', faulting, '(of', len(threads), '):')
    for frame in frames[:28]:
        index = frame.get('imageIndex', -1)
        image = images[index].get('name', '?') if 0 <= index < len(images) else '?'
        print('   ', image, frame.get('symbol', hex(frame.get('imageOffset', 0))))
    return 0


if __name__ == '__main__':
    sys.exit(main())
