-- AHBOT account
-- Fixed account ID: 915
-- This account is used by AuctionHouseBot.

INSERT INTO account
    (id, username, battlenet_account, sha_pass_hash,
     sessionkey, v, s, token_key,
     email, reg_mail, joindate, last_ip,
     failed_logins, locked, lock_country, online,
     expansion, mutetime, mutereason, muteby,
     locale, os, recruiter, project_member_id,
     rank, staff_id, vp, dp,
     isactive, activation, invited_by, inv_friend_acc,
     rewarded, flags, gmlevel, active_realm_id,
     online_mute_timer, active_mute_id, project_verified,
     cash, project_is_free, project_is_temp,
     project_unban_count, project_passchange, project_vote_time,
     project_hwid)
VALUES
    (915, 'AHBOT', '', '',
     '', '', '', '',
     '', '', CURRENT_TIMESTAMP, '127.0.0.1',
     0, 0, '00', 0,
     4, 0, '', '',
     0, 'Win', 0, 0,
     NULL, NULL, NULL, 0,
     NULL, NULL, '', '',
     0, 0, 0, 0,
     0, 0, 0,
     0, 0, 0,
     0, 0, 0,
     '')
ON DUPLICATE KEY UPDATE
    username = 'AHBOT';

-- Make sure future automatically assigned account IDs stay above AHBOT.
ALTER TABLE account AUTO_INCREMENT = 916;
