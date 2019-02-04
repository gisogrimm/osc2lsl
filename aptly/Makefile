# Debian Packages are moved through a couple of stages in this
# Makefile.  Let's give these stages names:
#
# supply:
# The packages stored here have just been built. Location is
# ./mha/tools/packaging/deb/hoertech/$SYSTEM/ regardless of $BRANCH_NAME.
# The packages here may be new or they may be rebuilds of
# existing versions (e.g. if someone clicks build-now while
# there is no new revision). Rebuilds of existing versions
# will not be used, but will cause an update of the timestamp of the respective
# files in storage.  New packages are copied to storage
#
# storage:
# The packages here are kept persistently across builds.
# Old packets (timestamp older than some threshold) will be deleted for
# branch development.
# Storage location:
# /var/lib/jenkins/packages-for-aptly/$BRANCH_NAME/$SYSTEM/ on the host,
# which is mounted to /packages/$BRANCH_NAME/$SYSTEM/ in the container.
#
# database:
# All packages for the current $BRANCH_NAME in storage are imported into
# fresh aptly databases during each update, one such database per $SYSTEM.
# Aptly scans the package metadata during this import. The databases are
# used during the update, and deleted again when the container gets deleted.
#
# snapshot:
# An unmutable view of the current state of the database, used for publication.
# We create one such snapshot per database, therefore one such snapshot per
# $SYSTEM.  These snapshots are also destroyed after the update when the
# container gets deleted.
#
# localrepo:
# apt-repository directory structure containing the packages and also several
# files with package lists, hashes, and signatures. Created by aptly from the
# snapshots. All $SYSTEM-specific snapshots are merged together into one
# localrepo. localepo is a directory inside the container, $HOME/.aptly/public,
# and destroyed when the container is deleted.
#
# remoterepo:
# apt-repository directory structure containing the packages and also several
# files with package lists, hashes, and signatures. Created by aptly from the
# snapshots. All $SYSTEM-specific snapshots are merged together into one
# localrepo. remoterepo is a directory on a remote webserver and can be
# modified via SFTP. to adapt remoterepo to the state of the localrepo,
# first all files on remoterepo are deleted that are not debian packages.
# Then, the localrepo is mirrored to the remoterepo while ignoring timestamps
# and deleting files in the remoterepo that are not present in localrepo.

# There will by $SYSTEM subdirectories below this directory.
# These subdirectories then contain the package files.
SUPPLY_DIR = ../packaging/pack/

# There will be $SYSTEM subdirectories below this directory.
STORAGE_DIR = /packages_tascar/$(BRANCH_NAME)/

# SYSTEM are the ubuntu distribution names (like bionic) that
# we have packages for in storage
SYSTEM = $(notdir $(wildcard $(SUPPLY_DIR)/*))

# How many days to keep debian packages in storage that are superceded by a
# newer version
RETENTION = 28

all: pruned-storage-$(BRANCH_NAME)

system:
	echo $(SYSTEM)

# Delete debian packages in storage older than RETENTION days
pruned-storage-%: updated-storage-%
	@echo uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
	@echo Begin pruning storage...
	@echo nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
	find $(STORAGE_DIR) -name "*.deb" -type f -mtime +$(RETENTION) -delete
	-rmdir $(STORAGE_DIR)/*   #  delete empty subdirs if there are any
	@echo uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
	@echo Storage pruning finished.
	@echo nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn

# Never delete old packages in the master database
pruned-storage-master: updated-storage-master
	@echo uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
	@echo "Keep all existing packages on branch master"
	@echo nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn

# copy the $SYSTEM/packages.deb tree without overwriting existing package files.
# then, update the timestamps for all files that are now in the supply to
# prevent deletion of latest files because they might be too old.
updated-storage-$(BRANCH_NAME):
	@echo uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
	@echo Begin updating storage...
	@echo nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
	mkdir -p $(STORAGE_DIR)$(SYSTEM)
	cp -anv $(SUPPLY_DIR)* $(STORAGE_DIR)
	cd $(SUPPLY_DIR) && find . -name \*.deb -exec touch $(STORAGE_DIR){} \;
	@echo uuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuuu
	@echo Storage update finished.
	@echo nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn
