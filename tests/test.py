#!/usr/bin/python3

import json
import os
import signal
import subprocess
import sys
import time

lib = "lime"
paths = [
    "test1",
    "test2",
    "test3",
    "test4",
    "test5",
]

isjson = {
    "test5": True,
}

CXX = os.environ.get("CXX")
if CXX is None:
    print("ERROR: No CXX environment variable found! Please read tests/README.txt")
    exit(1)

WAIT = 1


def moduleflags() -> list[str]:
    pkgconfig = subprocess.Popen(
        ["pkg-config", lib, "--cflags", "--libs"],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    stdout, _ = pkgconfig.communicate()
    data = stdout.decode("utf-8").strip()

    if len(data) == 0 or pkgconfig.returncode != 0:
        print(f"[ERROR] failed to get pkg-config data for lib: {lib}")
        exit(1)

    flags = data.split()
    print(f"[INFO] got module flags: {' '.join(flags)}")
    return flags


def compile(name: str, flags: list[str]):
    file = f"{name}/{name}.cc"
    gcc = subprocess.Popen(
        [
            f"{CXX}",
            "-std=c++23",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-o",
            f"{name}.out",
            file,
        ]
        + flags,
        stderr=subprocess.PIPE,
        stdout=subprocess.DEVNULL,
    )

    _, stderr = gcc.communicate()
    if gcc.returncode != 0:
        print(f"[ERROR] failed to compile file: {file}")
        print(stderr.decode("utf-8"))
        exit(1)

    print(f"[INFO] compiled: {file}")


def curl(path: str) -> list[str]:
    file = f"{path}/curl.txt"
    fcurl = open(file, "r")
    res = []

    for line in fcurl.read().splitlines():
        cmd = ["curl"]
        for args in line.split(" "):
            cmd.append(args.rstrip())

        curl = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
        stdout, _ = curl.communicate()

        res.append(stdout.decode("utf-8"))

    print(f"[INFO] curl: {file}")
    return res


def runexe(exe: str):
    return subprocess.Popen(
        [f"./{exe}.out"],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        start_new_session=True,
    )


def answers(path: str) -> list[str]:
    fans = open(f"{path}/ans.txt")
    print(f"[INFO] read answers from: {path}/ans.txt")
    return fans.read().splitlines()


def test(path: str, flags: list[str]):
    compile(path, flags)
    proc = runexe(path)
    time.sleep(WAIT)  # wait for exe to startup
    got = curl(path)

    os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
    proc.wait()
    print(f"[INFO] killed exe: {path}.out")

    expected = answers(path)
    if len(expected) != len(got):
        print(f"[ERROR] expected count: {len(expected)} != got count: {len(got)}")
        exit(1)

    i = 0
    while i < len(expected):
        e = expected[i].rstrip()
        g = got[i].rstrip()
        error = False

        if path in isjson:
            if json.loads(e) != json.loads(g):
                error = True
        elif e != g:
            error = True

        if error:
            print("[ERROR] test failed")
            print(f"\texpected: {expected[i]}")
            print(f"\tgot: {got[i]}")
            exit(1)

        i += 1

    print(f"[INFO] passed: {path}")


flags = moduleflags()

args = sys.argv[1:]

if len(args) > 0:
    for arg in args:
        test(arg, flags)
else:
    for path in paths:
        test(path, flags)

print("[INFO] All test passed")
