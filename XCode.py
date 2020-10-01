def mapping(left_matrix, right_matrix):
    left_mapping(left_matrix)


def left_mapping(left_matrix):
    col = 0
    mapping_list = []
    n = len(left_matrix)
    for row in range(len(left_matrix)):
        right_part_row = row + 1
        right_part_col = (n - 1) - col
        row_sequence = []

        row_sequence = first_pass(row_sequence, row, n)
        # print("first pass seq: ", row_sequence)
        row_sequence = second_pass(row_sequence, right_part_row, right_part_col, n)
        # print("second pass seq: ", row_sequence)
        mapping_list.append(row_sequence)
    print("Final Mapped List: ", mapping_list)
    return mapping_list


def first_pass(row_sequence, row, n):
    col = 0
    while row > -1 and col < n:
        if row != n - 1:
            row_sequence.append(tuple((row, col)))
        row -= 1
        col += 1
    return row_sequence


def second_pass(row_sequence, row, col, n):
    while row < (n - 1) and col > -1:
        row_sequence.append(tuple((row, col)))
        row += 1
        col -= 1
    return row_sequence


def input(N):
    left_matrix = [["(" + str(i) + "," + str(j) + ")" for i in range(N)] for j in range(N)]
    right_matrix = left_matrix
    print(left_matrix)
    return left_matrix, right_matrix


if __name__ == '__main__':
    left_matrix, right_matrix = input(7)
    mapping_list = mapping(left_matrix, right_matrix)
