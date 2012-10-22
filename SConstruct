AddOption('--test',
        dest='test',
        type='string',
        nargs=1,
        action='store',
        help='Type of test build to do')

AddOption('--count',
        dest='count',
        action='store_true',
        default=False,
        help='Count the lines of code, instead of building') #TODO: finish option

AddOption('--doc',
        dest='doc',
        action='store_true',
        default=False,
        help='Create Documentation')

AddOption('--lib',
        dest='lib',
        action='store_true',
        default=False,
        help='Create library files')

AddOption('--release',
        dest='release',
        action='store_true',
        default=False,
        help='Create release optimized executable')

SConscript(['SConslave'])
