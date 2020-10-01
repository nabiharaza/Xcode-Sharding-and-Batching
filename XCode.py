def mapping():
    pass


def input(N):
    left_matrix = [["(" + str(i) + "," + str(j) + ")" for i in range(N)] for j in range(N)]

    print(left_matrix)
    return left_matrix


if __name__ == '__main__':
    left_matrix = input(5)
