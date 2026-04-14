from conftest import epitar

USAGE = "Usage: epitar -[xcvh] <file.tar> [<files>]\n"
ERR = "epitar: invalid or missing option\nTry './epitar -h' for more information.\n"

def test_help():
    res = epitar(["-h"])

    assert res.returncode == 0
    assert USAGE in res.stdout

def test_help_x():
    res = epitar(["-h", "-x", "test.tar"])

    assert res.returncode == 0
    assert USAGE in res.stdout

def test_help_combined():
    res = epitar(["-xh", "test.tar"])

    assert res.returncode == 0
    assert USAGE in res.stdout

def test_help_bad():
    res = epitar(["-ah", "test.tar"])

    assert res.returncode == 0
    assert USAGE in res.stdout

def test_help_bad_two():
    res = epitar(["-a", "-h", "test.tar"])

    assert res.returncode == 0
    assert USAGE in res.stdout

def test_wrong():
    res = epitar(["-test", "test", "oui"])
    assert res.returncode == 1
    assert ERR in res.stderr

def test_wrong_after_good():
    res = epitar(["-xv", "-a", "test", "oui"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_wrong_before_good():
    res = epitar(["-a", "-cv", "test", "oui"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_wrong_betw_good():
    res = epitar(["-c", "-a", "-v", "test", "oui"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_empty():
    res = epitar([])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_no_opt():
    res = epitar(["archive.tar"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_extract_archive():
    res = epitar(["-xc", "test.tar"])
    
    assert res.returncode == 1
    assert ERR in res.stderr

def test_extract_archive_sep():
    res = epitar(["-x","-c", "test.tar"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_extract_archive_sep_rev():
    res = epitar(["-c","-x", "test.tar"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_extract_archive_sep_verbose():
    res = epitar(["-c", "-v", "-x", "test.tar"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_no_name():
    res = epitar(["-x"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_no_name_second():
    res = epitar(["-c"])

    assert res.returncode == 1
    assert ERR in res.stderr

def test_verbose():
    res = epitar(["-v", "test.tar"])
    assert res.returncode == 1
    assert ERR in res.stderr
