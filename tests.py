#!/usr/bin/python

from os import walk, remove, rmdir, mknod, mkdir, listdir
from os.path import join, exists, split, sep as fssep
from unittest import TestCase, main
from subprocess import Popen
from time import sleep

from rdiff_backup.Main import Main

class RdiffBackupTestMeta(type):
    
    TEST_DATA_DIRECTORY = 'tests'
    TEST_RDIFF_DIRECTORY = 'tests_backup'
    TEST_MOUNT_DIRECTORY = 'tests_mount'
    EXECUTABLE = './rdiff-backup-fs'
    UNMOUNT_EXECUTABLE = 'fusermount'
    
    def __new__(Meta, name, bases, classdict):
        fixtures = [(name, attr) for name, attr in classdict.items() 
                                 if name.startswith('fixture')]
        for name, attr in fixtures:
            for option, sufix in (('-f', 'full'), ('-n', 'necessary')):
                test = Meta.build_test(attr, option)
                classdict['test' + name[len('fixture'):] + '_' + sufix] = test
            del classdict[name]
        classdict['TEST_DATA_DIRECTORY'] = Meta.TEST_DATA_DIRECTORY
        classdict['TEST_RDIFF_DIRECTORY'] = Meta.TEST_RDIFF_DIRECTORY
        classdict['TEST_MOUNT_DIRECTORY'] = Meta.TEST_MOUNT_DIRECTORY
        Class = type.__new__(Meta, name, bases, dict(classdict))
        return Class
            
    @classmethod
    def build_test(Meta, fixture, option):
        def test(self):
            mkdir(Meta.TEST_DATA_DIRECTORY)
            Meta.create_mount_directory()
            for revision in fixture:
                for path, content in revision.items():
                    Meta.create_dirs(path)
                    file = open(join(Meta.TEST_DATA_DIRECTORY, path), 'w')
                    file.write(content)
                    file.close()
                Main([Meta.TEST_DATA_DIRECTORY, Meta.TEST_RDIFF_DIRECTORY])
                remove_directory(Meta.TEST_DATA_DIRECTORY)
                sleep(1)
            Meta.run_fs(option)
            Meta.verify(self, fixture)
        return test
    
    @classmethod
    def create_dirs(Meta, path):
        path = path.split(fssep)
        current_path = ''
        for step in path[:-1]:
            current_path += step
            mkdir(join(Meta.TEST_DATA_DIRECTORY, current_path))
        
    @classmethod
    def verify(Meta, self, fixture):
        revisions = sorted(listdir(Meta.TEST_MOUNT_DIRECTORY))
        for files, directory in zip(fixture, revisions):
            for path, content in files.items():
                full_path = join(Meta.TEST_MOUNT_DIRECTORY, directory, path)
                file = open(full_path)
                read_content = file.read()
                file.close()
                self.assertEqual(content, read_content)

    @classmethod
    def create_mount_directory(Meta):
        try:
            mkdir(Meta.TEST_MOUNT_DIRECTORY)
        except OSError as e:
            if e.errno != 17:
                raise

    @classmethod
    def run_fs(Meta, option):
        Popen([Meta.EXECUTABLE, Meta.TEST_MOUNT_DIRECTORY, 
               Meta.TEST_RDIFF_DIRECTORY, option]).wait()
        
    def unmount_fs(Meta):
        Popen([Meta.UNMOUNT_EXECUTABLE, '-u', 
               Meta.TEST_MOUNT_DIRECTORY]).wait()
        

class RdiffBackupTestCase(TestCase):
    __metaclass__ = RdiffBackupTestMeta
    
    def tearDown(self):
        self.__class__.unmount_fs()
        remove_directory(self.TEST_DATA_DIRECTORY, only_content=False)
        remove_directory(self.TEST_RDIFF_DIRECTORY, only_content=False)


class FlatTestCase(RdiffBackupTestCase):
    
    fixture_single_file = [
        {'file': 'content'},
        {'file': 'new content'}
    ]
    
    fixture_two_files = [
        {'file1': 'content1', 'file2': 'content2'},
        {'file1': 'new content 1', 'file2': 'new content 2'}
    ]
    
    fixture_adding_files = [
        {'file1': 'content1'},
        {'file1': 'content1', 'file2': 'content2'},
        {'file1': 'content1', 'file2': 'content2', 'file3': 'content3'},
        {'file1': 'content1', 'file2': 'content2', 'file3': 'content3',
         'file4': 'content4'}
    ]
    

class NestedTestCase(RdiffBackupTestCase):
    
    fixture_single_file = [
        {'dir/file': 'content'},
        {'dir/file': 'new content'}
    ]
    

class MultipleRepoTestCase(RdiffBackupTestCase):
    pass


def remove_directory(path, only_content=True):
    for root, dirs, files in walk(path, topdown=False):
        for name in files:
            remove(join(root, name))
        for name in dirs:
            rmdir(join(root, name))
    if not only_content:
        try:
            rmdir(path)
        except OSError:
            # directory may not be yet created
            pass
        
if __name__ == "__main__":
    main()
