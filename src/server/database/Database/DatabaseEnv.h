﻿/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DATABASEENV_H
#define DATABASEENV_H

#include "Define.h"
#include "DatabaseWorkerPool.h"

#include "Implementation/LoginDatabase.h"
#include "Implementation/CharacterDatabase.h"
#include "Implementation/WorldDatabase.h"

#include "Field.h"
#include "PreparedStatement.h"
#include "QueryCallback.h"
#include "QueryResult.h"
#include "Transaction.h"

/// world 数据库的访问器
TC_DATABASE_API extern DatabaseWorkerPool<WorldDatabaseConnection> WorldDatabase;
/// character 数据库的访问器
TC_DATABASE_API extern DatabaseWorkerPool<CharacterDatabaseConnection> CharacterDatabase;
/// realm/login 数据库的访问器
TC_DATABASE_API extern DatabaseWorkerPool<LoginDatabaseConnection> LoginDatabase;

#endif
