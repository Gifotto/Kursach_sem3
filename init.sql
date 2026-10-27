CREATE EXTENSION IF NOT EXISTS pgcrypto;

-- Схема в 3НФ: 5 таблиц, каждый факт хранится в одном месте.
--   roles          - справочник ролей (user / admin)
--   users          - пользователи (роль - внешний ключ)
--   difficulties   - справочник уровней сложности
--   games          - сыгранные партии (рекорды)
--   login_history  - журнал входов в аккаунты

-- 1. Роли
CREATE TABLE roles (
    id   SERIAL PRIMARY KEY,
    name VARCHAR(20) NOT NULL UNIQUE
);
INSERT INTO roles (name) VALUES ('user'), ('admin');

-- 2. Пользователи
CREATE TABLE users (
    id            SERIAL PRIMARY KEY,
    nickname      VARCHAR(20) NOT NULL,
    password_hash TEXT        NOT NULL,
    role_id       INTEGER     NOT NULL REFERENCES roles(id)
);
-- ники не повторяются (без учёта регистра)
CREATE UNIQUE INDEX users_nickname_uq ON users (lower(nickname));

-- 3. Уровни сложности (holes - число пустых клеток, множитель - для очков)
CREATE TABLE difficulties (
    id               SERIAL PRIMARY KEY,
    name             VARCHAR(20) NOT NULL UNIQUE,
    holes            INTEGER NOT NULL CHECK (holes BETWEEN 1 AND 60),
    score_multiplier INTEGER NOT NULL CHECK (score_multiplier > 0)
);
INSERT INTO difficulties (name, holes, score_multiplier)
VALUES ('easy', 35, 1), ('medium', 45, 2), ('hard', 52, 3);

-- 4. Партии (рекорды)
CREATE TABLE games (
    id            SERIAL PRIMARY KEY,
    user_id       INTEGER   NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    difficulty_id INTEGER   NOT NULL REFERENCES difficulties(id),
    score         INTEGER   NOT NULL CHECK (score >= 0),
    time_seconds  INTEGER   NOT NULL CHECK (time_seconds >= 0),
    played_at     TIMESTAMP NOT NULL DEFAULT now()
);
CREATE INDEX games_user_idx ON games(user_id);

-- 5. Журнал входов
CREATE TABLE login_history (
    id        SERIAL PRIMARY KEY,
    user_id   INTEGER   NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    logged_at TIMESTAMP NOT NULL DEFAULT now()
);
CREATE INDEX login_history_user_idx ON login_history(user_id);

-- Администратор: ник admin, пароль Admin#2024 (смените при желании)
INSERT INTO users (nickname, password_hash, role_id)
VALUES ('admin', crypt('Admin#2024', gen_salt('bf')), (SELECT id FROM roles WHERE name = 'admin'));
