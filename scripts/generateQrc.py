#!/usr/bin/python2.7
# coding=utf-8
import os

from path import require_file
from verbosity import print_info, print_very_important_info, set_verbosity

path =                  os.path.join(os.getcwd(), '..', 'src')
output_file =           'lunaInstallerApp.qrc'
recursive_directories = ['lunaInstallerApp']
exclude_files =         ['electronLunaInstaller', 'libnode.so', 'consoleInstaller',
                         'electronLunaInstaller.exe', 'node.dll', 'consoleInstaller.exe']
files_counter =         0

def run(source_path, output_path):
    print_very_important_info('Script will execute in path: ' + os.path.abspath(source_path) +
          '\nOutput will be written to: ' + os.path.abspath(output_path))

    output = '<RCC>\n'

    # Check files recursively in directories 
    for root_dir in recursive_directories:
        for root, dirs, files in os.walk(os.path.join(source_path, root_dir)):
            directory = root[len(source_path):].replace('\\', '/')
            directory = generate_qrc_dir_path(directory)
            print_info('Searching files (recursively) in: ' + directory)
            if len(directory) == 1 or (len(directory) > 1 and directory[1] != '.'):
                output += generate_filtered_files(files, directory)

    output += '</RCC>\n'

    with open(output_path, 'w') as file_:
        file_.write(output)

    print_very_important_info('Found ' + str(files_counter) + ' files!')
    print_very_important_info('File successfully saved to ' + os.path.abspath(output_path))


def generate_qrc_dir_path(directory):
    if not directory.startswith('/'):
        directory = '/' + directory
    return directory


def generate_filtered_files(files, directory):
    global files_counter
    directory = generate_qrc_dir_path(directory)
    is_section_empty = True
    output = ''
    for f in files:
        if f not in exclude_files:
            insert_filename = directory[1:]
            if directory != '/':
                insert_filename += '/'
            insert_filename += f
            if is_section_empty:
                output += '\t<qresource prefix="%s">\n' % directory
                is_section_empty = False
            output += '\t\t<file alias="%s">%s</file>\n' % (f, insert_filename)
            files_counter += 1

    if not is_section_empty:
        output += '\t</qresource>\n'
    return output


def main():
    run(".", output_file)

if __name__ == '__main__':
    main()
