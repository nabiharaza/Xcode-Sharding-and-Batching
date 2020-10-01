def input(N):
    left_matrix = [["(" + str(j) + "," + str(i) + ")" for i in range(N)] for j in range(N)]
    right_matrix = left_matrix
    print("Input:\n",left_matrix)
    print("--------------------------------------------------------------------------------------------------------\n")
    return left_matrix, right_matrix


def mapping(left_matrix, right_matrix):
    mapped_left_matrix = left_mapping(left_matrix)
    # print("Final Left Matrix Mapped: ", mapped_left_matrix)

    ## right matrix to be done
    mapped_right_matrix = right_mapping(right_matrix)

    ## parity to be calculated - make a dictonary , key = row (n-2) and value is XOR

    return mapped_left_matrix


def left_mapping(left_matrix):
    print("Left Mapping")
    col = 0
    mapping_list = []
    n = len(left_matrix)
    for row in range(len(left_matrix)):
        right_part_row = row + 1
        right_part_col = (n - 1) - col
        row_sequence = []

        row_sequence = left_matrix_first_pass(row_sequence, row, n)
        # print("first pass seq: ", row_sequence)
        row_sequence = left_matrix_second_pass(row_sequence, right_part_row, right_part_col, n)
        # print("second pass seq: ", row_sequence)
        print("Left matrix mapping seq: ", row_sequence)
        mapping_list.append(row_sequence)
    print("--------------------------------------------------------------------------------------------------------\n")
    return mapping_list


def left_matrix_first_pass(row_sequence, row, n):
    col = 0
    while row > -1 and col < n:
        if row != n - 1:
            row_sequence.append(tuple((row, col)))
        row -= 1
        col += 1
    return row_sequence


def left_matrix_second_pass(row_sequence, row, col, n):
    while row < (n - 1) and col > -1:
        row_sequence.append(tuple((row, col)))
        row += 1
        col -= 1
    return row_sequence


def right_matrix_first_pass(row_sequence, row, n):
    col = 0
    while row < n and col < n:
        if row != n - 2:
            row_sequence.append(tuple((row, col)))
            col += 1
        row += 1
    return row_sequence


def right_matrix_second_pass(row_sequence, row, col, n):
    parityFlag = False
    while row > -1 and col > -1:
        if row == n - 2 and parityFlag == False:
            row = row + 1
            parityFlag = True
        elif parityFlag == True and row == n - 2:
            row -= 1
        row_sequence.append(tuple((row, col)))

        row -= 1
        col -= 1
    return row_sequence


def right_mapping(right_matrix):
    print("Right Mapping")
    col = 0
    mapping_list = []
    n = len(right_matrix)
    for row in range(len(right_matrix)):
        right_part_row = row - 1
        right_part_col = (n - 1) - col
        row_sequence = []
        if row != n - 1:
            row_sequence = right_matrix_first_pass(row_sequence, row, n)
        # print("first pass seq: ", row_sequence)
        row_sequence = right_matrix_second_pass(row_sequence, right_part_row, right_part_col, n)
        # print("second pass seq: ", row_sequence)
        print("Right matrix mapping seq: ", row_sequence)
        mapping_list.append(row_sequence)
    print("--------------------------------------------------------------------------------------------------------\n")
    return mapping_list


def rotate(left_matrix, right_matrix):
    rotate_left_matrix(left_matrix)
    rotate_right_matrix(right_matrix)


def rotate_left_matrix(left_matrix):
    print("Rotate Left Matrix")
    whole_column_matrix = []

    for col in range(len(left_matrix)):
        # create column vectors
        col_sequence = [item[col] for item in left_matrix]

        # Rotate every column vector by i = (n-1) - c
        i = len(left_matrix) - 1 - col
        col_sequence = col_sequence[i:] + col_sequence[:i]
        whole_column_matrix.append(col_sequence)

    final_left_rotated_matrix = []

    # merge columns to produce matrix
    for row in range(len(whole_column_matrix)):
        row_sequence = [item[row] for item in whole_column_matrix]
        final_left_rotated_matrix.append(row_sequence)
        print(row_sequence)
    print("--------------------------------------------------------------------------------------------------------\n")
    return final_left_rotated_matrix


def rotate_right_matrix(right_matrix):
    print("Right Rotate Matrix")
    whole_column_matrix = []
    for col in range(len(right_matrix)):
        # create column vectors
        col_sequence = [item[col] for item in right_matrix]

        # swap nth element with n-1th element
        swap = col_sequence[len(right_matrix) - 1]
        col_sequence[len(right_matrix) - 1] = col_sequence[len(right_matrix) - 2]
        col_sequence[len(right_matrix) - 2] = swap

        # Rotate every column vector by i
        i = col
        col_sequence = col_sequence[i:] + col_sequence[:i]
        whole_column_matrix.append(col_sequence)

    final_right_rotated_matrix = []
    # merge columns to produce matrix
    for row in range(len(whole_column_matrix)):
        row_sequence = [item[row] for item in whole_column_matrix]
        final_right_rotated_matrix.append(row_sequence)
        print(row_sequence)
    print("--------------------------------------------------------------------------------------------------------\n")
    return final_right_rotated_matrix

def recovery(cols):
    pass

def getFailedNodes(cols):
    pass


if __name__ == '__main__':
    left_matrix, right_matrix = input(5)
    mapped_left_matrix = mapping(left_matrix, right_matrix)
    rotated_matrix = rotate(left_matrix, right_matrix)
