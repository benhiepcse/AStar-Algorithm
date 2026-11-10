from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
import zipfile

from check_submission import inspect


class Packaging(unittest.TestCase):
    student = "2353150"
    names = [f"2353150/{name}" for name in (
        "PathNode.h", "PathNode.cpp", "Algo.h", "Algo.cpp", "main.cpp",
        "2353150.pdf",
    )]

    def test_correct_listing(self):
        self.assertEqual(inspect(self.names, self.student), [])

    def test_each_missing(self):
        for name in self.names:
            with self.subTest(name=name):
                remaining = [entry for entry in self.names if entry != name]
                self.assertIn(("MISSING", name), inspect(remaining, self.student))

    def test_wrong_filename(self):
        names = self.names[:-1] + ["2353150/123.pdf"]
        self.assertIn(("FILE_NAME", "2353150/123.pdf"), inspect(names, self.student))

    def test_wrong_folder(self):
        self.assertIn(
            ("FOLDER_NAME", "wrong/Algo.cpp"),
            inspect(["wrong/Algo.cpp"], self.student),
        )

    def test_structure(self):
        for name in ("Algo.cpp", "2353150/extra/Algo.cpp", "../Algo.cpp"):
            with self.subTest(name=name):
                self.assertIn(("STRUCTURE", name), inspect([name], self.student))

    def test_fake_rar(self):
        with tempfile.TemporaryDirectory() as work:
            path = Path(work) / "wrong.rar"
            with zipfile.ZipFile(path, "w") as archive:
                archive.writestr("dummy.txt", "test")

            checker = Path(__file__).with_name("check_submission.py")
            result = subprocess.run(
                [sys.executable, str(checker), self.student, str(path)],
                capture_output=True, text=True,
            )
            self.assertEqual(result.returncode, 1)
            self.assertIn("FILE_NAME:", result.stdout)
            self.assertIn("FORMAT:", result.stdout)


if __name__ == "__main__":
    unittest.main()
