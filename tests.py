#!/usr/bin/python

from os import walk, remove, rmdir, mknod, mkdir
from os.path import join, exists, split
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
            classdict['test' + name[len('fixture'):]] = Meta.build_test(attr)
            del classdict[name]
        Class = type.__new__(Meta, name, bases, dict(classdict))
        return Class
            
    @classmethod
    def build_test(Meta, fixture):
        def test(self):
            mkdir(Meta.TEST_DATA_DIRECTORY)
            Meta.create_mount_directory()
            for revision in fixture:
                for path, content in revision.items():
                    file = open(join(Meta.TEST_DATA_DIRECTORY, path), 'w')
                    file.write(content)
                    file.close()
                Main([Meta.TEST_DATA_DIRECTORY, Meta.TEST_RDIFF_DIRECTORY])
                remove_directory(Meta.TEST_DATA_DIRECTORY)
                sleep(1)
            Meta.run_fs()
            Meta.unmount_fs()
            rmdir(Meta.TEST_DATA_DIRECTORY)
            remove_directory(Meta.TEST_RDIFF_DIRECTORY, only_content=False)
        return test

    @classmethod
    def create_mount_directory(Meta):
        try:
            mkdir(Meta.TEST_MOUNT_DIRECTORY)
        except OSError as e:
            if e.errno != 17:
                raise

    @classmethod
    def run_fs(Meta):
        Popen([Meta.EXECUTABLE, Meta.TEST_MOUNT_DIRECTORY, 
               Meta.TEST_RDIFF_DIRECTORY])
        
    @classmethod
    def unmount_fs(Meta):
        Popen([Meta.UNMOUNT_EXECUTABLE, '-u', Meta.TEST_MOUNT_DIRECTORY])


class SimpleTestCase(TestCase):
    __metaclass__ = RdiffBackupTestMeta
    
    fixture_single_file = [
        {'file': 'content'},
        {'file': 'new content'}
    ]
    
    #def test_some(self):
    #    print self.EXECUTABLE



def remove_directory(path, only_content=True):
    for root, dirs, files in walk(path, topdown=False):
        for name in files:
            remove(join(root, name))
        for name in dirs:
            rmdir(join(root, name))
    if not only_content:
        rmdir(path)
        
if __name__ == "__main__":
    main()
