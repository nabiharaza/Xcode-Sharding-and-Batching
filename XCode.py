def mapping(left_matrix, right_matrix):
    mapped_left_matrix = left_mapping(left_matrix)
    # print("Final Left Matrix Mapped: ", mapped_left_matrix)

    ## right matrix to be done
    mapped_right_matrix = right_mapping(right_matrix)
    ## parity to be calculated - make a dictonary , key = row (n-2) and value is XOR

    return mapped_left_matrix


def left_mapping(left_matrix):
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
        mapping_list.append(row_sequence)
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
    while row > -1 and col > -1:
        row_sequence.append(tuple((row, col)))
        row -= 1
        col -= 1
    return row_sequence

def right_mapping(right_matrix):
    col = 0
    mapping_list = []
    n = len(right_matrix)
    for row in range(len(right_matrix)):
        right_part_row = row - 1
        right_part_col = (n - 1) - col
        row_sequence = []
        if row != n - 2:
            # row_sequence = right_matrix_first_pass(row_sequence, row, n)
            # print("first pass seq: ", row_sequence)
            row_sequence = right_matrix_second_pass(row_sequence, right_part_row, right_part_col, n)
            print("second pass seq: ", row_sequence)
            mapping_list.append(row_sequence)
    return mapping_list

def input(N):
    left_matrix = [["(" + str(j) + "," + str(i) + ")" for i in range(N)] for j in range(N)]
    right_matrix = left_matrix
    print(left_matrix)
    return left_matrix, right_matrix


def rotate_left_matrix(left_matrix):
    # make coloumn vector
    whole_column_matrix = []
    for col in range(len(left_matrix)):
        col_sequence = [item[col] for item in left_matrix]
        # print("Col Seq :", col_sequence)

        #now implemeting lgoic to rotate col vector by i = (n-1) - c

        i = len(left_matrix) - 1 - col
        col_sequence = col_sequence[i:] + col_sequence[:i]
        # print("rotated col seq :", col_sequence)
        # print("\n--------------------------")
        whole_column_matrix.append(col_sequence)

    final_left_rotated_matrix = []
    # Join back columns ro make matrix again
    for row in range(len(whole_column_matrix)):
        row_sequence = [item[row] for item in whole_column_matrix]
        # print(row_sequence)
        final_left_rotated_matrix.append(row_sequence)
    return final_left_rotated_matrix



def rotate_right_matrix(right_matrix):
    pass


def rotate(mapped_left_matrix):
    rotate_left_matrix(mapped_left_matrix)
    rotate_right_matrix(right_matrix)


if __name__ == '__main__':
    left_matrix, right_matrix = input(5)
    mapped_left_matrix = mapping(left_matrix, right_matrix)
    rotated_matrix = rotate(left_matrix)
