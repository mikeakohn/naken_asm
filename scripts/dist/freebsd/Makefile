# $FreeBSD$

PORTNAME=	naken_asm
PORTVERSION=	2017.04.07
DISTNAME=	naken_asm-2017-04-07
CATEGORIES=	devel
MASTER_SITES=	http://www.mikekohn.net/downloads/naken_asm/
PKGNAMEPREFIX=	naken_asm-

MAINTAINER=	mike@mikekohn.net
COMMENT=	Assembler/disassembler multiple CPUs.

#RUN_DEPENDS=	libreadline.so:devel/readline
USES=		gmake readline
GNU_CONFIGURE=	yes

do-install:
	@mkdir -p ${STAGEDIR}${PREFIX}
	@cp ${WRKSRC}/naken_asm ${STAGEDIR}${PREFIX}/bin/
	@cp ${WRKSRC}/naken_util ${STAGEDIR}${PREFIX}/bin/

.include <bsd.port.mk>

