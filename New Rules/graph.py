import matplotlib.pyplot as plt
import numpy as np


def new_graph(clusters, output_graph):
    colors = ['b', 'g', 'r', 'c', 'm', 'y', 'k']
    colors_len = len(colors)
    color_index = 0

    if clusters[0].n != 2:
        raise Exception("ILLEGAL: Plot only works with 2 Dimensions")
    fig = plt.figure()
    for c in clusters:
        color = colors[color_index]
        points_x = [p.coords[0] for p in c.points]
        points_y = [p.coords[1] for p in c.points]

        plt.plot(points_x, points_y, color + 'o')

        m = c.centroid.coords[0]
        b = c.centroid.coords[1]

        plt.plot(points_x, m * np.asarray(points_x) + b, color + '-')

        color_index = (color_index + 1) % colors_len
    plt.savefig(output_graph + "_clusters.png")
    # plt.show()
    plt.close(fig)


def graph_cost(error_values, output_graph):
    fig = plt.figure()
    plt.plot(range(1, len(error_values) + 1), error_values, 'bo')
    for i in range(len(error_values)):
        plt.text(i + 1, error_values[i], '(' + str(i + 1) + ', ' +
                 str(error_values[i]) + ')')
    plt.savefig(output_graph + "_cost.png")
    plt.close(fig)
