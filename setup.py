from distutils.core import (
    setup,
    Extension,
)


def main():
    setup(
        name="faststring",
        version="1.0.0",
        description="Use strings but enhanced!",
        author="Robin Quintero",
        author_email="rohaquinlop301@gmail.com",
        ext_modules=[Extension("faststring", ["src/faststring.c"])],
    )


if __name__ == "__main__":
    main()
