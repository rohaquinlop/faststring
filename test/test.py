import pytest
from faststring import (
    StringBuilder,
)


@pytest.mark.benchmark(group="concatenate")
def test_stringbuilder_concat(benchmark):
    def f():
        sb = StringBuilder()
        for _ in range(100000):
            sb += "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."

    benchmark(f)


@pytest.mark.benchmark(group="concatenate")
def test_nativestring_concat(benchmark):
    def f():
        string = ""
        for _ in range(100000):
            string += "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."

    benchmark(f)


@pytest.mark.benchmark(group="modify-index")
def test_stringbuilder_modifyindex(benchmark):
    def f():
        sb = StringBuilder()
        sb += "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."

        for _ in range(100000):
            sb[10] = "a"

    benchmark(f)


@pytest.mark.benchmark(group="modify-index")
def test_nativestring_modifyindex(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."

        for _ in range(100000):
            string = string[:10] + "a" + string[11:]

    benchmark(f)
