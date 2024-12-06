import pytest
from faststring import MString

NUM_ITERATIONS = 100000


# Concatenation Benchmarks
@pytest.mark.benchmark(group="concatenate")
def test_mstring_concat(benchmark):
    def f():
        sb = MString()
        for _ in range(NUM_ITERATIONS):
            sb += "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."

    benchmark(f)


@pytest.mark.benchmark(group="concatenate")
def test_nativestring_concat(benchmark):
    def f():
        string = ""
        for _ in range(NUM_ITERATIONS):
            string += "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."

    benchmark(f)


# Modification by Index Benchmarks
@pytest.mark.benchmark(group="modify-index")
def test_mstring_modifyindex(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS):
            sb[10] = "a"

    benchmark(f)


@pytest.mark.benchmark(group="modify-index")
def test_nativestring_list_modifyindex(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            string = list(string)
            string[10] = "a"
            string = "".join(string)

    benchmark(f)


@pytest.mark.benchmark(group="modify-index")
def test_nativestring_modifyindex(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            string = string[:10] + "a" + string[11:]

    benchmark(f)


# Slicing Benchmarks
@pytest.mark.benchmark(group="slicing")
def test_mstring_slicing(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS):
            _slice = sb[10:20]

    benchmark(f)


@pytest.mark.benchmark(group="slicing")
def test_nativestring_slicing(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            _slice = string[10:20]

    benchmark(f)


# Slicing Modify Benchmarks
@pytest.mark.benchmark(group="slicing-modify")
def test_mstring_slicing_modify(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS):
            slice = sb[10:20]
            fr = sb[:10]
            sc = sb[21:]
            sb = sb[:10] + sb[21:]
            sb = fr + slice + sc

    benchmark(f)


@pytest.mark.benchmark(group="slicing-modify")
def test_nativestring_slicing_modify(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            slice = string[10:20]
            fr = string[:10]
            sc = string[21:]
            string = string[:10] + string[21:]
            string = fr + slice + sc

    benchmark(f)


# Contains Benchmarks
@pytest.mark.benchmark(group="contains")
def test_mstring_contains(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS):
            "dolorem" in sb

    benchmark(f)


@pytest.mark.benchmark(group="contains")
def test_nativestring_contains(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            "dolorem" in string

    benchmark(f)


# Find Benchmarks
@pytest.mark.benchmark(group="find")
def test_mstring_find(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS):
            sb.find("dolorem")

    benchmark(f)


@pytest.mark.benchmark(group="find")
def test_nativestring_find(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            string.find("dolorem")

    benchmark(f)


# Clear Benchmarks
@pytest.mark.benchmark(group="clear")
def test_mstring_clear(benchmark):
    def f():
        for _ in range(NUM_ITERATIONS):
            sb = MString(
                "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
            )
            sb.clear()

    benchmark(f)


@pytest.mark.benchmark(group="clear")
def test_nativestring_clear(benchmark):
    def f():
        for _ in range(NUM_ITERATIONS):
            _string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
            _string = ""

    benchmark(f)


# Insert Benchmarks
@pytest.mark.benchmark(group="insert")
def test_mstring_insert(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS):
            sb.insert(10, "a")

    benchmark(f)


@pytest.mark.benchmark(group="insert")
def test_nativestring_insert(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            string = string[:10] + "a" + string[10:]

    benchmark(f)


@pytest.mark.benchmark(group="insert")
def test_nativestring_list_insert(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS):
            string = list(string)
            string.insert(10, "a")
            string = "".join(string)

    benchmark(f)


@pytest.mark.benchmark(group="multiply")
def test_mstring_multiply(benchmark):
    def f():
        sb = MString(
            "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        )
        for _ in range(NUM_ITERATIONS // 10**4):
            sb *= 2

    benchmark(f)


@pytest.mark.benchmark(group="multiply")
def test_nativestring_multiply(benchmark):
    def f():
        string = "Neque porro quisquam est qui dolorem ipsum quia dolor sit amet, consectetur, adipisci velit..."
        for _ in range(NUM_ITERATIONS // 10**4):
            string *= 2

    benchmark(f)
