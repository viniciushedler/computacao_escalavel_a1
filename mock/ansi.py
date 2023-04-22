'''
    ANSI color code
'''

class ANSI():
    '''
        ANSI color code class
    '''
    color_name2int = {
        'black': 30,
        'red' : 91,
        'green' : 92,
        'yellow' : 93,
        'blue' : 94,
        'white': 97,
    }

    @staticmethod
    def colored_str(string, color_name):
        '''
            return colored string
        '''
        return f"\033[0;{ANSI.color_name2int[color_name]};40m{string}\033[m"

    @staticmethod
    def colored_print(string, color_name="white"):
        '''
            print colored string
        '''
        print(f"\033[0;{ANSI.color_name2int[color_name]};40m{string}\033[m", end="")
