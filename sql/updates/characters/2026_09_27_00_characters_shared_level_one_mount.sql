-- Heart of the Aspects: grant the shared ground/flying mount to every account
-- with characters. Account spells load on every character at login.
-- AutomaticRiding.cpp handles future accounts and grants riding/licenses.
INSERT INTO account_spell (account, spell, active, disabled)
SELECT DISTINCT account, 110051, 1, 0 FROM characters
ON DUPLICATE KEY UPDATE active = 1, disabled = 0;
