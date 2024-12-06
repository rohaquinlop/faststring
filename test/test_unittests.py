from faststring import MString


def test_mstring_creation():
    ms = MString("Hello, World!")
    assert ms.to_string() == "Hello, World!"
    assert len(ms) == 13


def test_mstring_append():
    ms = MString("Hello, World!")
    ms.append(" How are you?")
    assert ms.to_string() == "Hello, World! How are you?"
    assert len(ms) == 26


def test_mstring_concatenate():
    ms = MString("Hello, World!")
    ms += " How are you?"
    assert ms.to_string() == "Hello, World! How are you?"
    assert len(ms) == 26


def test_mstring_concatenate_mstrings():
    ms1 = MString("Hello, ")
    ms2 = MString("World!")
    ms = ms1 + ms2
    assert ms.to_string() == "Hello, World!"
    assert len(ms) == 13


def test_mstring_concatenate_mstring_native():
    ms = MString("Hello, ")
    ns = "World!"
    ms += ns
    assert ms.to_string() == "Hello, World!"
    assert len(ms) == 13


def test_mstring_modify_index():
    ms = MString("Hello, World!")
    ms[0] = "h"
    assert ms.to_string() == "hello, World!"
    assert len(ms) == 13


def test_mstring_modify_slice():
    ms = MString("Hello, World!")
    ms[0:5] = "Hi"
    assert ms.to_string() == "Hi, World!"
    assert len(ms) == 10


def test_mstring_slicing():
    ms = MString("Hello, World!")
    _slice = ms[0:5]
    assert _slice == "Hello"
    assert len(_slice) == 5
    assert len(ms) == 13


def test_mstring_contains():
    ms = MString("Hello, World!")
    assert "Hello" in ms
    assert "World" in ms
    assert "Hello, World!" in ms
    assert "Hello, World! How are you?" not in ms
    assert "Hi" not in ms
    assert "World!" in ms


def test_mstring_iter():
    ms = MString("Hello, World!")
    for i, c in enumerate(ms):
        assert c == ms[i]


def test_mstring_reverse():
    ms = MString("Hello, World!")
    ms.reverse()
    assert ms.to_string() == "!dlroW ,olleH"
    assert len(ms) == 13


def test_mstring_clear():
    ms = MString("Hello, World!")
    ms.clear()
    assert ms.to_string() == ""
    assert len(ms) == 0


def test_mstring_insert():
    ms = MString("Hello, World!")
    ms.insert(5, "!")
    assert ms.to_string() == "Hello!, World!"
    assert len(ms) == 14


def test_mstring_find():
    ms = MString("Hello, World!")
    assert ms.find("World") == 7
    assert ms.find("Hello") == 0
    assert ms.find("Hello, World!") == 0
    assert ms.find("Hi") == -1
    assert ms.find("World!") == 7
    assert ms.find("World!!") == -1


def test_mstring_multiply():
    ms = MString("Hello, World!")
    ms = ms * 3
    assert ms.to_string() == "Hello, World!Hello, World!Hello, World!"
    assert len(ms) == 39


def test_mstring_inplace_multiply():
    ms = MString("Hello, World!")
    ms *= 3
    assert ms.to_string() == "Hello, World!Hello, World!Hello, World!"
    assert len(ms) == 39


def test_mstring_replace():
    ms = MString("Hello, World!")
    ms.replace("Hello", "Hi")
    assert ms.to_string() == "Hi, World!"
    assert len(ms) == 10
