import io
import unittest

import hackassembler


class Test_HackAssembler(unittest.TestCase):
    def test_basic_parse_without_any_symbols(self):
        hackassembler.assemble(".\\tests\\add\\add.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\add\\add.hack")),
            list(io.open(".\\tests\\add\\add_ref.hack")))

    def test_parse_with_label_symbols(self):
        hackassembler.assemble(".\\tests\\max\\maxl.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\max\\maxl.hack")),
            list(io.open(".\\tests\\max\\max_ref.hack")))

        hackassembler.assemble(".\\tests\\rect\\rectl.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\rect\\rectl.hack")),
            list(io.open(".\\tests\\rect\\rect_ref.hack")))

        hackassembler.assemble(".\\tests\\pong\\pongl.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\pong\\pongl.hack")),
            list(io.open(".\\tests\\pong\\pong_ref.hack")))

    def test_parse_with_all_symbols(self):
        hackassembler.assemble(".\\tests\\max\\max.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\max\\max.hack")),
            list(io.open(".\\tests\\max\\max_ref.hack")))

        hackassembler.assemble(".\\tests\\rect\\rect.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\rect\\rect.hack")),
            list(io.open(".\\tests\\rect\\rect_ref.hack")))

        hackassembler.assemble(".\\tests\\pong\\pong.asm")
        self.assertListEqual(
            list(io.open(".\\tests\\pong\\pong.hack")),
            list(io.open(".\\tests\\pong\\pong_ref.hack")))


if __name__ == '__main__':
    #unittest.main(argv=['first-arg-is-ignored'], exit=False)
    unittest.main()
