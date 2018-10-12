ALL_CSV=$(wildcard *.csv)

VERSION=$(shell cat ../version)
DATE=$(shell LANG=en_EN date "+%d %B %Y")

all:
	rm -Rf pack man
	$(MAKE) man/osc2lsl.1.gz
	mkdir -p pack
	for c in $(ALL_CSV); do (cd .. && htchdebian-mkdeb packaging/$$c $(VERSION) ); done
	mv ../*.deb ./pack
	for c in $(ALL_CSV:.csv=); do ls pack/$${c}_*.deb || return 1; done

ver:
	echo "$(VERSION)"

man/%.1.gz: ../doc/%.1
	mkdir -p man
	cat $< | sed -e "s/-VERSION-/$(VERSION)/g" -e "s/-DATE-/\"$(DATE)\"/g" |gzip > $@
