MAKEFILES	:=	sysmod overlay
TARGETS		:= $(foreach dir,$(MAKEFILES),$(CURDIR)/$(dir))

# the below was taken from atmosphere + switch-examples makefile
export VERSION := 1.5.8

ifneq ($(strip $(shell git symbolic-ref --short HEAD 2>/dev/null)),)
export GIT_BRANCH := $(shell git symbolic-ref --short HEAD)
else
export GIT_BRANCH := notbranch
endif

ifeq ($(strip $(shell git status --porcelain 2>/dev/null)),)
export GIT_REVISION := $(GIT_BRANCH)-$(shell git rev-parse --short HEAD)
export VERSION_DIRTY := $(VERSION)
export VERSION_WITH_HASH := $(VERSION)-$(shell git rev-parse --short HEAD)
else
export GIT_REVISION := $(GIT_BRANCH)-$(shell git rev-parse --short HEAD)-dirty
export VERSION_DIRTY := $(VERSION)-dirty
export VERSION_WITH_HASH := $(VERSION)-$(shell git rev-parse --short HEAD)-dirty
endif

export BUILD_DATE := -DDATE_YEAR=\"$(shell date +%Y)\" \
					-DDATE_MONTH=\"$(shell date +%m)\" \
					-DDATE_DAY=\"$(shell date +%d)\" \
					-DDATE_HOUR=\"$(shell date +%H)\" \
					-DDATE_MIN=\"$(shell date +%M)\" \
					-DDATE_SEC=\"$(shell date +%S)\" \

export CUSTOM_DEFINES := -DVERSION=\"v$(VERSION)\" \
					-DGIT_BRANCH=\"$(GIT_BRANCH)\" \
					-DGIT_REVISION=\"$(GIT_REVISION)\" \
					-DVERSION_DIRTY=\"$(VERSION_DIRTY)\" \
					-DVERSION_WITH_HASH=\"$(VERSION_WITH_HASH)\" \
					$(BUILD_DATE)

all: $(TARGETS)
	@mkdir -p out/
	@cp -R sysmod/out/* out/
	@cp -R overlay/out/* out/

.PHONY: $(TARGETS)

$(TARGETS):
	@$(MAKE) -C $@

clean:
	@rm -rf out
	@for i in $(TARGETS); do $(MAKE) -C $$i clean || exit 1; done;

dist: all
	@for i in $(TARGETS); do $(MAKE) -C $$i dist || exit 1; done;
	@echo making dist ...

	@rm -f sys-patch.zip
	@cd out; zip -r ../sys-patch.zip ./*; cd ../
