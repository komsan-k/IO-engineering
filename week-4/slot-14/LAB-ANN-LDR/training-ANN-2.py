weights = model.get_weights()

W1 = weights[0]
b1 = weights[1]
W2 = weights[2]
b2 = weights[3]


def print_arduino_array_1d(
    name,
    values,
    size_name
):

    print(
        f"float {name}[{size_name}] ="
    )

    print("{")

    for i, value in enumerate(
        values
    ):

        comma = (
            ","
            if i < len(values) - 1
            else ""
        )

        print(
            f"  {value:.8f}f{comma}"
        )

    print("};")


def print_arduino_array_2d(
    name,
    matrix,
    row_name,
    col_name
):

    print(
        f"float {name}"
        f"[{row_name}]"
        f"[{col_name}] ="
    )

    print("{")

    for i in range(
        matrix.shape[0]
    ):

        row = ", ".join(
            f"{value:.8f}f"
            for value in matrix[i]
        )

        comma = (
            ","
            if i < matrix.shape[0] - 1
            else ""
        )

        print(
            f"  {{ {row} }}{comma}"
        )

    print("};")


print(
    "\n// W1 shape = 1 x 4"
)

print_arduino_array_1d(
    "W1",
    W1[0],
    "HIDDEN_SIZE"
)


print(
    "\n// b1 shape = 4"
)

print_arduino_array_1d(
    "b1",
    b1,
    "HIDDEN_SIZE"
)


print(
    "\n// W2 shape = 4 x 3"
)

print_arduino_array_2d(
    "W2",
    W2,
    "HIDDEN_SIZE",
    "OUTPUT_SIZE"
)


print(
    "\n// b2 shape = 3"
)

print_arduino_array_1d(
    "b2",
    b2,
    "OUTPUT_SIZE"
)
