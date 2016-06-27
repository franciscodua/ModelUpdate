class Log:
    def __init__(self, file_name):
        self.file_name = file_name
        self.log_list = []
        self.n_items = 0

    def log(self, record):
        self.log_list.append(record)
        self.n_items += 1
        if self.n_items >= 1000:
            self.write()
        print record

    def write(self):
        with open(self.file_name, 'a') as f:
            for record in self.log_list:
                f.write(record + '\n')
        self.n_items = 0
        self.log_list = []
