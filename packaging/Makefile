ALL_CSV=$(wildcard *.csv)

VERSION=$(shell cat ../version)

all:
	rm -Rf pack
	mkdir -p pack
	for c in $(ALL_CSV); do (cd .. && htchdebian-mkdeb packaging/$$c $(VERSION) ); done
	mv ../*.deb ./pack
	for c in $(ALL_CSV:.csv=); do ls pack/$${c}_*.deb || return 1; done

ver:
	echo "$(VERSION)"

