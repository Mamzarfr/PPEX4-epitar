import os

from conftest import epitar, tar, FILES

def test_nonexistent(tmp_path):
    res = epitar(["-x", "test.tar"], tmp_path)

    assert res.returncode == 3
    assert "epitar: error extracting tarball test.tar" in res.stderr

def test_nonexistent_base(tmp_path):
    test = tmp_path / "test.tar"
    res = epitar(["-x", str(test)], tmp_path)

    assert res.returncode == 3
    assert "epitar: error extracting tarball test.tar" in res.stderr

def test_bad_checksum(tmp_path, create_files):
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    with open(tar_path, "r+b") as f:
        f.seek(148)
        f.write(b"67676767")

    res = epitar(["-x", tar_path], tmp_path)

    assert res.returncode == 2
    assert "epitar: bad checksum" in res.stderr

def test_bad_magic(tmp_path, create_files):
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    with open(tar_path, "r+b") as f:
        f.seek(257)
        f.write(b"tests")

    res = epitar(["-x", tar_path], tmp_path)
    assert res.returncode == 2
    assert "epitar: bad checksum" in res.stderr

def test_truncate(tmp_path):
    text = "x" * 1000
    (tmp_path / "test.txt").write_text(text)
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "test.txt"], tmp_path)

    os.truncate(tar_path, 676)

    out = tmp_path / "out"
    out.mkdir()
    res = epitar(["-x", tar_path], out)
    
    assert res.returncode == 3
    assert "epitar: error extracting tarball test.tar" in res.stderr
    assert (out / "test.txt").read_text() == ""

def test_simple(tmp_path, create_files):
    tar_path = str(tmp_path / "songs.tar")
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "songs"], tmp_path)

    output = tmp_path / "out"
    output.mkdir()
    res = epitar(["-x", tar_path], output)
    assert res.returncode == 0

    for f in FILES:
        ref = tmp_path / f
        out = output / f
        assert out.read_text() == ref.read_text()

def test_empty_file(tmp_path):
    (tmp_path / "test.txt").write_text("")
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "test.txt"], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    res = epitar(["-x", str(tar_path)], out)

    assert res.returncode == 0
    assert (out / "test.txt").read_text() == ""

def test_empty_dir(tmp_path):
    (tmp_path / "test").mkdir()
    tar_path = tmp_path / "test.tar"
    tar(["--format=ustar", "-cf", tar_path, "-b", "1", "test"], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    res = epitar(["-x", str(tar_path)], out)
    
    assert res.returncode == 0
    assert (out / "test").is_dir()

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
