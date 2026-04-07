import os

from conftest import epitar, tar

def test_help():
    res = epitar(["-h"])
    assert res.returncode == 0
    assert "Usage: epitar -[xcvh] <file.tar> [<files>]" in res.stdout

def test_help_tricky():
    res = epitar(["-h", "-x", "test.tar"])
    assert res.returncode == 0

def test_wrong_opts():
    res = epitar(["-test", "test", "oui.tar"])
    assert res.returncode == 1
    assert "epitar: invalid or missing option\nTry './epitar -h' for more information.\n" in res.stderr

def test_noarg():
    res = epitar([])
    assert res.returncode == 1
    assert "epitar: invalid or missing option\nTry './epitar -h' for more information.\n" in res.stderr

def test_noopt():
    res = epitar(["archive.tar"])
    assert res.returncode == 1
    assert "epitar: invalid or missing option\nTry './epitar -h' for more information.\n" in res.stderr

def test_bad_checksum(tmp_path, create_files):
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    with open(tar_path, "r+b") as f:
        f.seek(148)
        f.write(b"67676767")

    res = epitar(["-x", tar_path], tmp_path)
    assert res.returncode == 2
    assert "epitar: bad checksum" in res.stderr

def test_nonexistent(tmp_path):
    res = epitar(["-x", "test.tar"], tmp_path)
    assert res.returncode == 3
    assert "epitar: error extracting tarball test.tar" in res.stderr

def test_truncate(tmp_path):
    text = "x" * 1000
    (tmp_path / "test.txt").write_text(text)
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "test.txt"], tmp_path)

    os.truncate(tar_path, 670)

    out = tmp_path / "out"
    out.mkdir()
    res = epitar(["-x", tar_path], out)
    assert res.returncode == 3
    assert f"epitar: error extracting tarball {tar_path}" in res.stderr
    assert (out / "test.txt").read_text() == ""

def test_magic(tmp_path, create_files):
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    with open(tar_path, "r+b") as f:
        f.seek(257)
        f.write(b"magic")

    res = epitar(["-x", tar_path], tmp_path)
    assert res.returncode == 3
    assert f"epitar: error extracting tarball {tar_path}" in res.stderr

def test_simple(tmp_path, create_files):
    tar_path = str(tmp_path / "songs.tar")
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    output = tmp_path / "out"
    output.mkdir()
    res = epitar(["-x", tar_path], output)
    assert res.returncode == 0

    files = [
        "songs/Test - song1",
        "songs/Non - test2",
        "songs/Test3 - oui",
        "songs/TestDir/Test4 - t4",
        "songs/TestDir/NestedDir/Test5 - t5",
    ]

    for f in files:
        ref = tmp_path / f
        out = output / f
        assert out.read_text() == ref.read_text()

def test_verbose(tmp_path, create_files):
    tar_path = tmp_path / "songs.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    ref_dir = tmp_path / "ref"
    ref_dir.mkdir()

    res = epitar(["-xv", tar_path], out)
    ref = tar(["-xvf", tar_path], ref_dir)

    assert res.returncode == 0
    assert res.stdout == ref.stdout

def test_valid_flags(tmp_path, create_files):
    tar_path = str(tmp_path / "songs.tar")
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    out = tmp_path / "out"
    out2 = tmp_path / "out2"
    out3 = tmp_path / "out3"
    out4 = tmp_path / "out4"
    out.mkdir()
    out2.mkdir()
    out3.mkdir()
    out4.mkdir()

    r = epitar(["-v", "-x", tar_path], out)
    r2 = epitar(["-x", "-v", tar_path], out2)
    r3 = epitar(["-vx", tar_path], out3)
    r4 = epitar(["-xv", tar_path], out4)
    assert r.returncode == 0
    assert r2.returncode == 0
    assert r3.returncode == 0
    assert r4.returncode == 0
    assert r.stdout == r2.stdout == r3.stdout == r4.stdout