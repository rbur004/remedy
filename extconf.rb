require 'mkmf'
dir_config('tmpfile', './include', './lib') #optional include-path, optional lib-path
have_library('ar')
create_makefile('tmpfile')
