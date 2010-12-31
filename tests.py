#!/usr/bin/python

# TODO:
# - check for files, that should not exist

from os import walk, remove, rmdir, mknod, mkdir, listdir, stat
from os.path import join, exists, split, sep as fssep, isdir
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
                test = Meta.build_test(attr, option, Meta.verify)
                classdict['test' + name[len('fixture'):] + '_' + sufix] = test
            test = Meta.build_test(attr, '-l', Meta.verify_last)
            classdict['test' + name[len('fixture'):] + '_last'] = test
            del classdict[name]
        classdict['TEST_DATA_DIRECTORY'] = Meta.TEST_DATA_DIRECTORY
        classdict['TEST_RDIFF_DIRECTORY'] = Meta.TEST_RDIFF_DIRECTORY
        classdict['TEST_MOUNT_DIRECTORY'] = Meta.TEST_MOUNT_DIRECTORY
        Class = type.__new__(Meta, name, bases, dict(classdict))
        return Class
            
    @classmethod
    def build_test(Meta, fixture, option, verify_method):
        def test(self):
            mkdir(Meta.TEST_DATA_DIRECTORY)
            mkdir(Meta.TEST_RDIFF_DIRECTORY)
            Meta.create_mount_directory()
            for repo, data in fixture.items():
                for revision in data:
                    for path, content in revision.items():
                        Meta.create_dirs(path)
                        file = open(join(Meta.TEST_DATA_DIRECTORY, path), 'w')
                        file.write(content)
                        file.close()
                    backup_path = join(Meta.TEST_RDIFF_DIRECTORY, repo)
                    Main([Meta.TEST_DATA_DIRECTORY, backup_path])
                    remove_directory(Meta.TEST_DATA_DIRECTORY)
                    sleep(1)
            Meta.run_fs(fixture.keys(), option)
            verify_method(self, fixture)
        return test
        
    @classmethod
    def create_dirs(Meta, path):
        path = path.split(fssep)
        current_path = ''
        for step in path[:-1]:
            current_path = join(current_path, step)
            try:
                mkdir(join(Meta.TEST_DATA_DIRECTORY, current_path))
            except OSError: # the directory might already exist
                pass
        
    @classmethod
    def verify(Meta, self, fixture):
        self.assert_(len(fixture) > 0)
        if len(fixture) == 1: # single repo
            Meta.verify_revisions(self, Meta.TEST_MOUNT_DIRECTORY, 
                                  fixture.values()[0])
        else:
            for name, data in fixture.items():
                repo_path = join(Meta.TEST_MOUNT_DIRECTORY, name)
                Meta.verify_revisions(self, repo_path, data)
                    
    @classmethod
    def verify_revisions(Meta, self, repo_path, data):
        revisions = sorted(listdir(repo_path))
        previous = {}
        for files, directory in zip(data, revisions):
            for path, content in files.items():
                full_path = join(repo_path, directory, path)
                file = open(full_path)
                read_content = file.read()
                file.close()
                self.assertEqual(content, read_content)
            previous = set(previous.keys()) - set(files.keys())
            for path in previous:
                full_path = join(repo_path, directory, path)
                self.assertRaises(OSError, stat, path)
            previous = files
        
    @classmethod
    def verify_last(Meta, self, fixture):
        if len(fixture) == 1:
            Meta.verify_last_revisions(self, Meta.TEST_MOUNT_DIRECTORY,
                                       fixture.values()[0])
        else:
            for name, data in fixture.items():
                repo_path = join(Meta.TEST_MOUNT_DIRECTORY, name)
                Meta.verify_last_revisions(self, repo_path, data)
                
    @classmethod
    def verify_last_revisions(Meta, self, repo_path, data):
        for file, content in data[-1].items():
            path = join(repo_path, file)
            self.assert_(exists(path))
            self.assert_(isdir(path))

    @classmethod
    def create_mount_directory(Meta):
        try:
            mkdir(Meta.TEST_MOUNT_DIRECTORY)
        except OSError as e:
            if e.errno != 17:
                raise

    @classmethod
    def run_fs(Meta, repos, option):
        repo_paths = [join(Meta.TEST_RDIFF_DIRECTORY, repo) for repo in repos]
        Popen([Meta.EXECUTABLE, Meta.TEST_MOUNT_DIRECTORY] + 
               repo_paths + [option]).wait()
        
    def unmount_fs(Meta):
        Popen([Meta.UNMOUNT_EXECUTABLE, '-u', 
               Meta.TEST_MOUNT_DIRECTORY]).wait()
        

class RdiffBackupTestCase(TestCase):
    __metaclass__ = RdiffBackupTestMeta

    @staticmethod
    def reverse_revisions(repos):
        result = {}
        for name, data in repos.items():
            result[name] = list(data)
            result[name].reverse()
        return result

    
    def tearDown(self):
        self.__class__.unmount_fs()
        remove_directory(self.TEST_DATA_DIRECTORY, only_content=False)
        remove_directory(self.TEST_RDIFF_DIRECTORY, only_content=False)


class FlatTestCase(RdiffBackupTestCase):
    
    fixture_single_file = {
        'backup': [
            {'file': 'content'},
            {'file': 'new content'}
        ]
    }
    
    fixture_two_files = {
        'backup': [
            {'file1': 'content1', 'file2': 'content2'},
            {'file1': 'new content 1', 'file2': 'new content 2'}
        ]
    }
    
    fixture_adding_files = {
        'backup': [
            {'file1': 'content1'},
            {'file1': 'content1', 'file2': 'content2'},
            {'file1': 'content1', 'file2': 'content2', 'file3': 'content3'},
            {'file1': 'content1', 'file2': 'content2', 'file3': 'content3',
             'file4': 'content4'}
        ]
    }
    
    fixture_removing_files = RdiffBackupTestCase.reverse_revisions(fixture_adding_files)
        
class NestedTestCase(RdiffBackupTestCase):
    
    fixture_single_file = {
        'nested_backup': [
            {'dir/file': 'content'},
            {'dir/file': 'new content'}
        ]
    }
    
    fixture_two_files = {
        'nested_backup': [
            {'file': 'content', 'dir/file': 'content 2'},
            {'file': 'new content', 'dir/file': 'new content 2'}
        ]
    }
    
    fixture_adding_files = {
        'nested_backup': [
            {'file': '1'},
            {'file': '2', 'dir/file': '2'},
            {'file': '3', 'dir/file': '3', 'dir/dir/file': '3'},
            {'file': '4', 'dir/file': '4', 'dir/dir/file': '4', 
         '   dir/dir/dir/file': '4'}
        ]
    }
    
    fixture_removing_files = RdiffBackupTestCase.reverse_revisions(fixture_adding_files)
    

class MultipleRepoTestCase(RdiffBackupTestCase):

    fixture_single_file = {
        'first': [
            {'file': 'content'},
            {'file': 'new content'}
        ],
        'second': [
            {'file': 'content'},
            {'file': 'new content'}
        ]
    }


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
