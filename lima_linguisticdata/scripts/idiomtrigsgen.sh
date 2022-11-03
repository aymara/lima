# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

if [ -f idiomaticExpressionsTriggers-$1.xml ];
then
cp idiomaticExpressionsTriggers-$1.xml dicoIdiom.xml
else
cat <<EOF > dicoIdiom.xml
<IdiomaticOffset version="1.0" language="$1" >
</IdiomaticOffset>
EOF
fi
