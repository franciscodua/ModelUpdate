import socket


class HAProxyStats:
    """
    Communicate with HAProxy through its UNIX socket (e.g. show stat)
    """

    def __init__(self, sock_name):
        """
        :param sock_name: name (path/name) of unix socket
        """
        self.sock_name = sock_name

    def __read_csv_to_dict(self, csv_string):
        """
        Convert csv in string format into dictionary.
        csv package only reads from files (this reads from string)
        :param csv_string: csv file in string format
        :return: dictionary with stats (key is property and value is "value"
        """
        stats = dict()
        lines = csv_string.splitlines()
        names = lines[0].split(',')
        values = []

        for line in lines:
            line_split = line.split(',')
            if line_split[1] == "BACKEND":
                values = line_split
                break

        if len(names) != len(values):
            # TODO: improve message
            raise Exception("Stats parsing resulted in different number of columns")

        for i in range(len(names)):
            stats[names[i]] = values[i]

        return stats

    def get_stats(self):
        """
        Communicate with socket "show stat". Ask for stats, parse them, return
        :return: dictionary with stats
        """
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(self.sock_name)
        sock.send("show stat\n")

        received = ""

        while received == "":
            received = sock.recv(1024)

        stats = self.__read_csv_to_dict(received)

        sock.close()

        return stats
