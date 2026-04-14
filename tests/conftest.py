import os
import subprocess
import pytest

EXE = os.path.join(os.path.dirname(__file__), "..", "epitar")

FILES = [
    "songs/Test - song1",
    "songs/Non - test2",
    "songs/Test3 - oui",
    "songs/TestDir/Test4 - t4",
    "songs/TestDir/NestedDir/Test5 - t5",
]

@pytest.fixture
def create_files(tmp_path):
    d = tmp_path / "songs"
    d.mkdir()
    (d / "TestDir").mkdir()
    (d / "Test - song1").write_text("test1")
    (d / "Non - test2").write_text("test2")
    (d / "Test3 - oui").write_text("")
    (d / "TestDir" / "Test4 - t4").write_text("test4")
    (d / "TestDir" / "NestedDir").mkdir()
    (d / "TestDir" / "NestedDir" / "Test5 - t5").write_text("test5")

def epitar(args: list[str], path=None):
    return subprocess.run([EXE] + args, capture_output=True, text=True, cwd=path)

def tar(args: list[str], path=None):
    return subprocess.run(["tar"] + args, capture_output=True, text=True, cwd=path)
