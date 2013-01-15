remedy
======

A Ruby library, mapping to the BMC's Remedy V7.0 C-API using Ruby's C Extention API.

This code has been abandoned, as we have dropped Remedy in favour of a SaaS solution.
I'll leave the code here as an example of what could be done (good or bad).

Copy the libraries somewhere where Ruby will find them when it is running (e.g /usr/lib).
It doesn't matter about the include files.

To install

	ruby extconf.rb
	make
	sudo make install


To use.

	require 'ar'
	include 'ARMod'
	
	Ar.open(user,password,server,authString,localeinfo) do |ar|
		#do stuff, 
		#eg. 
		...
		begin
			fieldList = ar.ARGetEntry(schema, entryId, idList) #Nb. No control or status arguments.
		rescue Status => status
			puts status.to_s
			exit(1)
		end
		...
	end
		





