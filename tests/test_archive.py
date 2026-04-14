from conftest import FILES, epitar, tar


def test_empty(tmp_path):
    res = epitar(["-c", "test.tar"], tmp_path)

    assert res.returncode == 1
    assert "epitar: cowardly refusing to create an empty archive\n" in res.stderr
    assert not (tmp_path / "test.tar").exists()


def test_single_file(tmp_path):
    (tmp_path / "test.txt").write_text("Vous savez, moi je ne crois pas qu’il y ait de bonne ou de mauvaise situation. Moi, si je devais résumer ma vie aujourd’hui avec vous, je dirais que c’est d’abord des rencontres. Des gens qui m’ont tendu la main, peut-être à un moment où je ne pouvais pas, où j’étais seul chez moi. Et c’est assez curieux de se dire que les hasards, les rencontres forgent une destinée... Parce que quand on a le goût de la chose, quand on a le goût de la chose bien faite, le beau geste, parfois on ne trouve pas l’interlocuteur en face je dirais, le miroir qui vous aide à avancer. Alors ça n’est pas mon cas, comme je disais là, puisque moi au contraire, j’ai pu ; et je dis merci à la vie, je lui dis merci, je chante la vie, je danse la vie... je ne suis qu’amour ! Et finalement, quand des gens me disent « Mais comment fais-tu pour avoir cette humanité ? », je leur réponds très simplement que c’est ce goût de l’amour, ce goût donc qui m’a poussé aujourd’hui à entreprendre une construction mécanique... mais demain qui sait ? Peut-être simplement à me mettre au service de la communauté, à faire le don, le don de soi.")
    res = epitar(["-c", "test.tar", "test.txt"], tmp_path)
    ref = tar(["-tf", "test.tar"], tmp_path)

    assert res.returncode == 0
    assert "test.txt\n" in ref.stdout
    with (tmp_path / "test.txt").open("r") as f:
        assert f.read() == "Vous savez, moi je ne crois pas qu’il y ait de bonne ou de mauvaise situation. Moi, si je devais résumer ma vie aujourd’hui avec vous, je dirais que c’est d’abord des rencontres. Des gens qui m’ont tendu la main, peut-être à un moment où je ne pouvais pas, où j’étais seul chez moi. Et c’est assez curieux de se dire que les hasards, les rencontres forgent une destinée... Parce que quand on a le goût de la chose, quand on a le goût de la chose bien faite, le beau geste, parfois on ne trouve pas l’interlocuteur en face je dirais, le miroir qui vous aide à avancer. Alors ça n’est pas mon cas, comme je disais là, puisque moi au contraire, j’ai pu ; et je dis merci à la vie, je lui dis merci, je chante la vie, je danse la vie... je ne suis qu’amour ! Et finalement, quand des gens me disent « Mais comment fais-tu pour avoir cette humanité ? », je leur réponds très simplement que c’est ce goût de l’amour, ce goût donc qui m’a poussé aujourd’hui à entreprendre une construction mécanique... mais demain qui sait ? Peut-être simplement à me mettre au service de la communauté, à faire le don, le don de soi."


def test_single_dir(tmp_path):
    (tmp_path / "test").mkdir()
    res = epitar(["-c", "test.tar", "test"], tmp_path)
    ref = tar(["-tf", "test.tar"], tmp_path)

    assert res.returncode == 0
    assert "test/\n" in ref.stdout


def test_multiple(tmp_path):
    (tmp_path / "a").write_text("a")
    (tmp_path / "b").write_text("b")
    (tmp_path / "c").write_text("c")
    res = epitar(["-c", "test.tar", "a", "b", "c"], tmp_path)
    ref = tar(["-tf", "test.tar"], tmp_path)

    assert res.returncode == 0
    assert "a\n" in ref.stdout
    assert "b\n" in ref.stdout
    assert "c\n" in ref.stdout


def test_verbose(tmp_path, create_files):
    res = epitar(["-cv", "test.tar", "songs"], tmp_path)

    assert res.returncode == 0
    assert "songs/\n" in res.stdout
    assert "songs/TestDir/\n" in res.stdout
    assert "songs/TestDir/NestedDir/\n" in res.stdout
    for path in FILES:
        assert f"{path}\n" in res.stdout


def test_unable(tmp_path):
    (tmp_path / "test.txt").write_text("test")
    res = epitar(["-c", "test.tar", "test.txt", "nonexistant"], tmp_path)
    ref = tar(["-tf", "test.tar"], tmp_path)

    assert res.returncode == 3
    assert "epitar: unable to add file nonexistant to tarball test.tar" in res.stderr
    assert "test.txt\n" in ref.stdout


def test_self(tmp_path):
    (tmp_path / "a").write_text("a")
    (tmp_path / "b").write_text("b")
    res = epitar(["-c", "test.tar", "."], tmp_path)
    ref = tar(["-tf", "test.tar"], tmp_path)

    assert res.returncode == 0
    assert "epitar: test.tar is the archive; not dumped" in res.stderr
    assert "test.tar\n" not in ref.stdout


def test_empty_file(tmp_path):
    (tmp_path / "test.txt").write_text("")
    res = epitar(["-c", "out.tar", "test.txt"], tmp_path)
    ref = tar(["-tf", "out.tar"], tmp_path)

    assert res.returncode == 0
    assert "test.txt\n" in ref.stdout


def test_empty_dir(tmp_path):
    (tmp_path / "test").mkdir()
    res = epitar(["-c", "out.tar", "test"], tmp_path)
    ref = tar(["-tf", "out.tar"], tmp_path)

    assert res.returncode == 0
    assert "test/\n" in ref.stdout
