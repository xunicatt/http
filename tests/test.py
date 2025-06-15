#!/usr/bin/python
import os
import sys
import signal
import subprocess

lib = "http"
paths = [
    "test1",
    "test2",
    "test3",
    "test4",
    "test5",
]

def moduleflags() -> tuple[str, str, str]:
    pkgconfig = subprocess.Popen(
        [ "pkg-config", lib, "--cflags", "--libs" ],
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
    )
    stdout, _ = pkgconfig.communicate()
    data = stdout.decode('utf-8')

    if len(data) == 0 or pkgconfig.returncode != 0:
        print(f"[ERROR] failed to get pkg-config data for lib: {lib}")
        exit(1)

    incdir, libdir, libname = data.split(' ')
    libname = libname.rstrip()
    print(f"[INFO] got module flags: {incdir} {libdir} {libname}")
    return incdir, libdir, libname

def compile(name: str, incdir: str, libdir: str, libname: str):
    file = f"{name}/{name}.cc"
    gcc = subprocess.Popen(
        [
            "g++",
            "-std=c++23",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-o",
            f"{name}.out",
            file,
            incdir,
            libdir,
            libname,
        ],
        stderr=subprocess.PIPE,
        stdout=subprocess.DEVNULL,
    )

    _, stderr = gcc.communicate()
    if gcc.returncode != 0:
        print(f"[ERROR] failed to compile file: {file}")
        print(stderr.decode('utf-8'))
        exit(1)

    print(f"[INFO] compiled: {file}")

def curl(path: str) -> list[str]:
    file = f"{path}/curl.txt"
    fcurl = open(file, "r")
    res = []
    
    for line in fcurl.read().splitlines():
        cmd = [ "curl" ]
        for args in line.split(' '):
            cmd.append(args.rstrip())

        curl = subprocess.Popen(
            cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
        )
        stdout, _ = curl.communicate()

        res.append(stdout.decode('utf-8'))

    print(f"[INFO] curl: {file}")
    return res

def runexe(exe: str):
    return subprocess.Popen(
        [ f"./{exe}.out" ],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
        start_new_session=True,
    )

def answers(path: str) -> list[str]:
    fans = open(f"{path}/ans.txt")
    print(f"[INFO] read answers from: {path}/ans.txt")
    return fans.read().splitlines()

def test(path: str, incdir: str, libdir: str, libname: str):
    compile(path, incdir, libdir, libname)
    proc = runexe(path)
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
        if expected[i].rstrip() != got[i].rstrip():
            print(f"[ERROR] test failed")
            print(f"\texpected: {expected[i]}")
            print(f"\tgot: {got[i]}")
            exit(1)
        i += 1

    print(f"[INFO] passed: {path}")

incdir, libdir, libname = moduleflags()

args = sys.argv
if len(sys.argv) > 1:
    for arg in sys.argv[1:]:
        test(arg, incdir, libdir, libname)
else:
    for path in paths:
        test(path, incdir, libdir, libname)

print("[INFO] All test passed")
