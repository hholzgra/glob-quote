# Copyright (c) 1999, 2000 Sascha Schumann. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY SASCHA SCHUMANN ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
# EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
##############################################################################
# $Id$ 
#

LTLIBRARY_OBJECTS = $(LTLIBRARY_SOURCES:.c=.lo)

$(LTLIBRARY_NAME): $(LTLIBRARY_OBJECTS) $(LTLIBRARY_DEPENDENCIES)
	$(LINK) $(LTLIBRARY_LDFLAGS) $(LTLIBRARY_OBJECTS) $(LTLIBRARY_LIBADD)

$(LTLIBRARY_SHARED_NAME): $(LTLIBRARY_OBJECTS) $(LTLIBRARY_DEPENDENCIES)
	@test -d $(phplibdir) || $(mkinstalldirs) $(phplibdir)
	$(LINK) -avoid-version -module -rpath $(phplibdir) $(LTLIBRARY_LDFLAGS) $(LTLIBRARY_OBJECTS) $(LTLIBRARY_SHARED_LIBADD)
	$(SHLIBTOOL) --mode=install cp $@ $(phplibdir)

shared:
	@if test -z '$(LTLIBRARY_SHARED_NAME)'; then \
		echo "LTLIBRARY_SHARED_NAME is empty. Double check your Makefile.in"; \
	fi
	@if test '$(SHLIBTOOL)' != '$(LIBTOOL)'; then \
		$(MAKE) 'LIBTOOL=$(SHLIBTOOL)' $(LTLIBRARY_SHARED_NAME); \
	else \
		$(MAKE) $(LTLIBRARY_SHARED_NAME); \
	fi; \
