ALL_CSV=$(wildcard *.csv)

VERSION=$(shell cat ../version)

all:
	for c in $(ALL_CSV); do (cd .. && htchdebian-mkdeb packaging/$$c $(VERSION) ); done
	mv ../*.deb ./
	for c in $(ALL_CSV:.csv=); do ls $${c}_*.deb || return 1; done

ver:
	echo "$(VERSION)"

