#!/bin/sh

# This will generate a nice set of ctags for issuetracker
# Make sure that you have exuberant-ctags installed and not the
# lame GNU or BSD one
# add the following to your ~/.vimrc:
#    tags=~/.tags,~/.cpsl_tags,tags
#    path+=s,~/.cpsl_tags,tags
# Then to jup to an identifier put the cursor over it and type CTRL-]
# To jump to the next identifier type :tn
# You can also look up any tag by typing :tag foo
# You may want to add this directory to your path in vim:
# set path+=<issue-tracking repository>/**
# Happy tagging!


nice --adjustment=19 /usr/bin/ctags \
                           --fields=+lS \
                           --append=yes \
                           --language-force=java \
                           --langmap=JavaScript:.batch.library.configure \
                           --sort=yes \
                           --recurse=yes \
                           --exclude=.git \
                           --exclude=*.class \
                           --exclude=*.iml \
                           --exclude=*.eml \
                           -o ~/.cpsl_tags.tmp \
                           $(pwd)

mv ~/.cpsl_tags.tmp ~/.cpsl_tags
