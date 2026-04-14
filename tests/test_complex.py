from conftest import epitar, tar, FILES

def test_round_epitar(tmp_path, create_files):
    tarball = epitar(["-c", "test.tar", "songs"], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    (out / "out.tar").write_bytes((tmp_path / "test.tar").read_bytes())
    res = epitar(["-x", "out.tar"], out)

    assert tarball.returncode == 0
    assert res.returncode == 0
    for f in FILES:
        assert (out / f).read_text() == (tmp_path / f).read_text()

def test_round_epitar_tar(tmp_path, create_files):
    res = epitar(["-c", "out.tar", "songs"], tmp_path)
    out = tmp_path / "out"
    out.mkdir()
    ref = tar(["-xf", str(tmp_path / "out.tar")], out)

    assert ref.returncode == 0
    assert res.returncode == 0
    for f in FILES:
        assert (out / f).read_text() == (tmp_path / f).read_text()

def test_round_tar_epitar(tmp_path, create_files):
    tar_path = tmp_path / "out.tar"
    ref = tar(["--format=ustar", "-cf", tar_path, "songs"], tmp_path)
    out = tmp_path / "out"
    out.mkdir()
    res = epitar(["-x", str(tar_path)], out)

    assert ref.returncode == 0
    assert res.returncode == 0
    for f in FILES:
        assert (out / f).read_text() == (tmp_path / f).read_text()

def test_round_verbose(tmp_path, create_files):
    res = epitar(["-cv", "out.tar", "songs"], tmp_path)
    ref = tar(["--format=ustar", "-cvf", "ref.tar", "songs"], tmp_path)

    assert res.returncode == 0
    assert res.stdout == ref.stdout

def test_round_many(tmp_path):
    (tmp_path / "a.txt").write_text(" a ")
    (tmp_path / "b.txt").write_text("b" * 1000)
    (tmp_path / "c.txt").write_text("")
    res = epitar(["-c", "out.tar", "a.txt", "b.txt", "c.txt"], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    res2 = epitar(["-x", str(tmp_path / "out.tar")], out)
    
    assert res.returncode == 0
    assert res2.returncode == 0
    assert (out / "a.txt").read_text() == " a "
    assert (out / "b.txt").read_text() == "b" * 1000
    assert (out / "c.txt").read_text() == ""

def test_large(tmp_path):
    text = "Vous savez, moi je ne crois pas qu’il y ait de bonne ou de mauvaise situation. Moi, si je devais résumer ma vie aujourd’hui avec vous, je dirais que c’est d’abord des rencontres. Des gens qui m’ont tendu la main, peut-être à un moment où je ne pouvais pas, où j’étais seul chez moi. Et c’est assez curieux de se dire que les hasards, les rencontres forgent une destinée... Parce que quand on a le goût de la chose, quand on a le goût de la chose bien faite, le beau geste, parfois on ne trouve pas l’interlocuteur en face je dirais, le miroir qui vous aide à avancer. Alors ça n’est pas mon cas, comme je disais là, puisque moi au contraire, j’ai pu ; et je dis merci à la vie, je lui dis merci, je chante la vie, je danse la vie... je ne suis qu’amour ! Et finalement, quand des gens me disent « Mais comment fais-tu pour avoir cette humanité ? », je leur réponds très simplement que c’est ce goût de l’amour, ce goût donc qui m’a poussé aujourd’hui à entreprendre une construction mécanique... mais demain qui sait ? Peut-être simplement à me mettre au service de la communauté, à faire le don, le don de soi." * 15000
    (tmp_path / "test.txt").write_text(text)
    res = epitar(["-c", "out.tar", "test.txt"], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    res2 = epitar(["-x", str(tmp_path / "out.tar")], out)

    assert res.returncode == 0
    assert res2.returncode == 0
    assert (out / "test.txt").read_text() == text

def test_round_self(tmp_path):
    (tmp_path / "test1.txt").write_text("test1")
    (tmp_path / "test2.txt").write_text("test2")
    res = epitar(["-c", "test.tar", "."], tmp_path)

    out = tmp_path / "out"
    out.mkdir()
    res2 = epitar(["-x", str(tmp_path / "test.tar")], out)

    assert res.returncode == 0
    assert res2.returncode == 0
    assert not (out / "test.tar").exists()
    assert (out / "test1.txt").read_text() == "test1"
    assert (out / "test2.txt").read_text() == "test2"
