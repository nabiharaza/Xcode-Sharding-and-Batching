map_right_rotated_to_original = []
map_left_rotated_to_original = []
final_right_rotated_matrix = []
final_left_rotated_matrix = []


def input(matrix_dimensions):
    left_matrix = [[str(j) + "," + str(i) for i in range(matrix_dimensions)] for j in
                   range(matrix_dimensions)]
    right_matrix = left_matrix
    # print("Left Input:\n", left_matrix)
    # print("Right Input:\n", right_matrix)
    #
    # print("--------------------------------------------------------------------------------------------------------\n")
    return left_matrix, right_matrix


def mapping(left_matrix, right_matrix):
    mapped_left_matrix = left_mapping(left_matrix)
    # print("Final Left Matrix Mapped: ", mapped_left_matrix)

    mapped_right_matrix = right_mapping(right_matrix)

    ## parity to be calculated - make a dictonary , key = row (n-2) and value is XOR
    return mapped_left_matrix, mapped_right_matrix


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
        # print("Left matrix mapping seq: ", row_sequence)
        mapping_list.append(row_sequence)
    # print("--------------------------------------------------------------------------------------------------------\n")
    return mapping_list


def left_matrix_first_pass(row_sequence, row, n):
    col = 0
    while row > -1 and col < n:
        if row != n - 1:
            row_sequence.append([row, col])
        row -= 1
        col += 1
    return row_sequence


def left_matrix_second_pass(row_sequence, row, col, n):
    while row < (n - 1) and col > -1:
        row_sequence.append([row, col])
        row += 1
        col -= 1
    return row_sequence


def right_matrix_first_pass(row_sequence, row, n):
    col = 0
    while row < n and col < n:
        if row != n - 2:
            row_sequence.append([row, col])
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
        row_sequence.append([row, col])

        row -= 1
        col -= 1
    return row_sequence


def right_mapping(right_matrix):
    # print("Right Mapping")
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
    #     print("Right matrix mapping seq: ", row_sequence)
    #     mapping_list.append(row_sequence)
    # print("--------------------------------------------------------------------------------------------------------\n")
    return mapping_list


def rotate(left_matrix, right_matrix):
    final_left_rotated_matrix, map_left_rotated_to_original = rotate_left_matrix(left_matrix)
    final_right_rotated_matrix, map_right_rotated_to_original = rotate_right_matrix(right_matrix)

    return final_left_rotated_matrix, map_left_rotated_to_original, final_right_rotated_matrix, map_right_rotated_to_original


def rotate_left_matrix(left_matrix):
    # print("Rotate Left Matrix")
    whole_column_matrix = []

    tracker = []
    for col in range(len(left_matrix)):
        # create column vectors
        col_sequence = [row[col] for row in left_matrix]
        # implementing mapping/index_tracker
        index_tracker = [row for row in range(len(left_matrix))]

        # Rotate every column vector by i = (n-1) - c
        i = len(left_matrix) - 1 - col
        col_sequence = col_sequence[i:] + col_sequence[:i]
        index_tracker = index_tracker[i:] + index_tracker[:i]
        # print("row rotated", index_tracker)
        tracker.append(index_tracker)

        whole_column_matrix.append(col_sequence)

    final_left_rotated_matrix = []

    # merge columns to produce matrix
    for row in range(len(whole_column_matrix)):
        row_sequence = [item[row] for item in whole_column_matrix]
        final_left_rotated_matrix.append(row_sequence)
        # print(row_sequence)
    # print("--------------------------------------------------------------------------------------------------------\n")
    map_left_rotated_to_original, itr = [], 0
    for row in range(len(tracker)):
        row_sequence = []
        itr = 0
        for item in range(len(tracker)):
            row_sequence.append([tracker[item][row], itr])
            itr += 1
        map_left_rotated_to_original.append(row_sequence)
        # print(map_left_rotated_to_original[row])
    return final_left_rotated_matrix, map_left_rotated_to_original


def rotate_right_matrix(right_matrix):
    # print("Right Rotate Matrix")
    whole_column_matrix = []
    tracker = []

    for col in range(len(right_matrix)):
        # create column vectors
        col_sequence = [item[col] for item in right_matrix]
        # for mapping/index_tracer
        index_tracker = [row for row in range(len(left_matrix))]
        # print(index_tracker)

        # swap nth element with n-1th element
        swap = col_sequence[len(right_matrix) - 1]
        col_sequence[len(right_matrix) - 1] = col_sequence[len(right_matrix) - 2]
        col_sequence[len(right_matrix) - 2] = swap

        # Do the same swapping for the index_tracker
        swap_index_tracker = index_tracker[len(right_matrix) - 1]
        index_tracker[len(right_matrix) - 1] = index_tracker[len(right_matrix) - 2]
        index_tracker[len(right_matrix) - 2] = swap_index_tracker

        # Rotate every column vector by i
        i = col
        col_sequence = col_sequence[i:] + col_sequence[:i]
        whole_column_matrix.append(col_sequence)

        # Rotate every column vector by i for the tracker
        i = col
        index_tracker = index_tracker[i:] + index_tracker[:i]
        tracker.append(index_tracker)

    # print(tracker)
    final_right_rotated_matrix = []

    # merge columns to produce matrix
    for row in range(len(whole_column_matrix)):
        row_sequence = [item[row] for item in whole_column_matrix]
        final_right_rotated_matrix.append(row_sequence)
        print(row_sequence)

    map_right_rotated_to_original, itr = [], 0
    print("\nRight Mapped Matrix")
    for row in range(len(tracker)):
        row_sequence = []
        itr = 0
        for item in range(len(tracker)):
            row_sequence.append([tracker[item][row], itr])
            itr += 1
        map_right_rotated_to_original.append(row_sequence)
        # print(map_right_rotated_to_original[row])

    # print("--------------------------------------------------------------------------------------------------------\n")
    return final_right_rotated_matrix, map_right_rotated_to_original


def recovery_process(failed_vectors, left_failed_nodes, right_failed_nodes,
                     final_left_rotated_matrix, map_left_rotated_to_original, final_right_rotated_matrix,
                     map_right_rotated_to_original):
    # making a replica matrix where diagonals are set to zero and failed nodes are set to zero
    duplicate_rotated_left = final_left_rotated_matrix
    duplicate_rotated_right = final_right_rotated_matrix
    index = 0

    # mark the diagonals by 0
    while index < len(final_left_rotated_matrix):
        duplicate_rotated_left[index][index] = 'D,D'
        index = index + 1
    # mark the failed vectors by -1

    for column_vector in failed_vectors:
        for row in range(len(final_left_rotated_matrix)):
            # # leaving out the primary diagonals to be added in failed nodes
            if row != column_vector:
                duplicate_rotated_left[row][column_vector] = '-1,-1'
            else:
                duplicate_rotated_left[row][column_vector] = 'D,D'
    print("DL:,", duplicate_rotated_left)

    for column_vector in failed_vectors:
        for row in range(len(final_right_rotated_matrix)):
            # print(row)
            # leaving out the primary diagonals to be added in failed nodes
            if (row + column_vector) != (len(right_matrix) - 1):
                duplicate_rotated_right[row][column_vector] = '-1,-1'
            else:
                duplicate_rotated_right[row][column_vector] = 'D,D'
        print("duplicate ", duplicate_rotated_right[column_vector])

    step_one_recovery(duplicate_rotated_left, duplicate_rotated_right, map_right_rotated_to_original,
                      map_left_rotated_to_original, failed_vectors, right_failed_nodes, left_failed_nodes)


def step_one_recovery(duplicate_rotated_left, duplicate_rotated_right, map_right_rotated_to_original,
                      map_left_rotated_to_original, failed_vectors, right_failed_nodes, left_failed_nodes):
    for row in range(len(duplicate_rotated_left)):
        broken_count = 0
        diagonal_count = 0
        for column in range(len(duplicate_rotated_left)):

            if column in failed_vectors:
                if duplicate_rotated_left[row][column] == 'D,D':
                    diagonal_count += 1
                if duplicate_rotated_left[row][column] == '-1,-1':
                    broken_count += 1
                if broken_count > 1:
                    print('skipping row', row, column)
                    continue
                if broken_count == 1 and diagonal_count == 1:
                    duplicate_rotated_left[row][column] = 'R,R'
    print(duplicate_rotated_left)


def get_failed_nodes(failed_vectors, final_left_rotated_matrix, final_right_rotated_matrix):
    """
    Change the left_matrix to left rotated matrix and like wise for the right matrix too
    :param failed_vectors:
    :param left__matrix:
    :param right_matrix:
    :return:
    """
    left_failed_nodes = []
    right_failed_nodes = []

    for column_vector in failed_vectors:
        for row in range(len(final_left_rotated_matrix)):
            # # leaving out the primary diagonals to be added in failed nodes
            if row != column_vector:
                left_failed_nodes.append([row, column_vector])
    # print("left failed node:", left_failed_nodes)

    for column_vector in failed_vectors:
        for row in range(len(final_right_rotated_matrix)):
            # print(row)
            # leaving out the primary diagonals to be added in failed nodes
            if (row + column_vector) != (len(right_matrix) - 1):
                right_failed_nodes.append([row, column_vector])
    # print("Right failed node:", right_failed_nodes)
    return left_failed_nodes, right_failed_nodes


if __name__ == '__main__':
    left_matrix, right_matrix = input(5)
    mapped_left_matrix, mapped_right_matrix = mapping(left_matrix, right_matrix)
    final_left_rotated_matrix, map_left_rotated_to_original, final_right_rotated_matrix, map_right_rotated_to_original, = rotate(
        left_matrix, right_matrix)
    failed_vectors = [1, 3]
    left_failed_nodes, right_failed_nodes = get_failed_nodes(failed_vectors, final_left_rotated_matrix,
                                                             final_right_rotated_matrix)
    recovery_process(failed_vectors, left_failed_nodes, right_failed_nodes,
                     final_left_rotated_matrix, map_left_rotated_to_original, final_right_rotated_matrix,
                     map_right_rotated_to_original)
