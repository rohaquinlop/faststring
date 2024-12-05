from setuptools import (
    setup,
    Extension,
)


def main():
    setup(
        name="faststring",
        version="0.1.0",
        description="Python library for mutable string manipulation.",
        long_description=open("README.md").read(),
        long_description_content_type="text/markdown",
        license="MIT",
        author="Robin Quintero",
        author_email="rohaquinlop301@gmail.com",
        url="https://github.com/rohaquinlop/faststring",
        ext_modules=[Extension("faststring", ["src/faststring.c"])],
        classifiers=[
            "Programming Language :: Python :: 3",
            "License :: OSI Approved :: MIT License",
            "Operating System :: OS Independent",
        ],
        python_requires=">=3.11",
        install_requires=[],
        extras_require={
            "dev": [
                "pytest",
                "pytest-benchmark>=5.1.0",
                "setuptools",
            ]
        },
    )


if __name__ == "__main__":
    main()
