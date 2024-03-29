import socket


class HAProxyConnector:
    """
    Communicate with HAProxy through its UNIX socket (e.g. show stat)
    """

    def __init__(self, sock_name):
        """
        :param sock_name: name (path/name) of unix socket
        """
        self.sock_name = sock_name

    @staticmethod
    def __read_csv_to_dict(csv_string):
        """
        Convert csv in string format into dictionary.
        csv package only reads from files (this reads from string)
        :param csv_string: csv file in string format
        :return: dictionary with stats (key is property and value is "value"
        """
        backend_stats = dict()
        frontend_stats = dict()
        lines = csv_string.splitlines()
        names = lines[0].split(',')
        backend_values = []
        frontend_values = []

        for line in lines:
            line_split = line.split(',')
            if line_split[1] == "BACKEND":
                backend_values = line_split
                break
            if line_split[1] == "FRONTEND":
                frontend_values = line_split

        if len(names) != len(backend_values):
            # TODO: improve message
            raise Exception("Stats parsing resulted in different number of columns (" +
                            str(len(names)) + ", " + str(len(backend_values)) + ")")

        for i in range(len(names)):
            backend_stats[names[i]] = backend_values[i]
            frontend_stats[names[i]] = frontend_values[i]

        return backend_stats, frontend_stats

    def send_command(self, command):
        """
        send commands to HAProxy via UNIX socket
        :param command: string with command to be sent
        :return: result of executing command
        """
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(self.sock_name)
        sock.send(command + "\n")

        received = ""

        while received == "":
            received = sock.recv(10000)

        sock.close()

        return received

    def get_stats(self):
        """
        Communicate with socket "show stat". Ask for stats, parse them, return
        :return: dictionary with stats
        """
        received = self.send_command("show stat")

        backend_stats, frontend_stats = self.__read_csv_to_dict(received)

        return backend_stats, frontend_stats

    def disable_server(self, server_name):
        """
        Communicate with socket "disable server".
        Note: This command is restricted and can only be issued on sockets configured for level "admin"
        :param server_name: string with server name (e.g. lamp1)
        :return: void
        """
        command = "disable server servers/" + server_name

        self.send_command(command)

        return

    def enable_server(self, server_name):
        """
        Communicate with socket "enable server".
        Note: This command is restricted and can only be issued on sockets configured for level "admin"
        :param server_name: string with server name (e.g. lamp1)
        :return: void
        """
        command = "enable server servers/" + server_name

        self.send_command(command)

        return
