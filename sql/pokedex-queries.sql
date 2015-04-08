-- Travis J. Sanders
-- CS 275 Su14
-- 2014.08.06
-- Final Project

# SELECT ALL POKEMON, SHOW TYPES
SELECT pokedex.id, pokedex.name, T.name AS "Type 1", type.name AS "Type 2"
FROM pokedex
LEFT JOIN type ON type.id = pokedex.type2
INNER JOIN type T ON T.id = pokedex.type1;

# SELECT ALL POKEMON WITH MULIPLE TYPES
SELECT pokedex.id, pokedex.name, T.name AS "Type 1", type.name AS "Type 2"
FROM pokedex
INNER JOIN type ON type.id = pokedex.type2
INNER JOIN type T ON T.id = pokedex.type1;

# SELECT ALL POKEMON OF WATER TYPE
SELECT pokedex.id, pokedex.name, T.name AS "Type 1", type.name AS "Type 2"
FROM pokedex
LEFT JOIN type ON type.id = pokedex.type2
INNER JOIN type T ON T.id = pokedex.type1
WHERE type1 = (
  SELECT id FROM type
  WHERE type.name = "water"
  )
OR type2 = (
  SELECT id FROM type
  WHERE type.name = "water"
  );

# SELECT ALL TYPES WHICH ARE WEAK TO FIRE
SELECT *
FROM type_relation
WHERE a_type = (
  SELECT id FROM type
  WHERE type.name = "fire"
  )
AND relation = -1;

# SELECT ALL POKEMON WHICH ARE WEAK TO FIRE
SELECT * FROM pokedex
WHERE type1 IN (
  SELECT d_type FROM type_relation
  WHERE a_type = (
    SELECT id FROM type
    WHERE type.name = "fire"
    )
  AND relation = 1
  );

# SELECT ALL POKEMON WHICH ARE WEAK AGAINST PIKACHU
-- Is there a more efficient way to do this? "WHERE type1 OR type2 IN" didn't work
SELECT * FROM pokedex
WHERE type1 IN (
  SELECT d_type FROM type_relation
  WHERE relation = 1
  AND a_type = (
    SELECT type1 FROM pokedex
    WHERE name = "Pikachu"
    )
) OR type2 IN (
  SELECT d_type FROM type_relation
  WHERE relation = 1
  AND a_type = (
    SELECT type1 FROM pokedex
    WHERE name = "Pikachu"
    )
);

# ADD NEW POKEMON TYPE
INSERT INTO type (name) VALUES (["newtype"]);

# ADD NEW POKEMON TO POKEDEX
INSERT INTO pokedex (name, type1, type2) 
VALUES (["name"], [type1], [type2 or NULL]);

# ADD NEW POKEMON TO POKEDEX FROM TYPE NAME
INSERT INTO pokedex (name, type1) 
VALUES (["name"], (
	SELECT id FROM type
	WHERE type.name = ["type"]
	)
);

# CREATE NEW PLAYER
INSERT INTO player (name) VALUES (["username"]);

# CREATE A SPECIFIC POKEMON (DEFAULT LEVEL 5)
SET AUTOCOMMIT=0;
INSERT INTO pokemon (poke_id, player_id)
VALUES ([145], [1]);
COMMIT;

# DELETE A SPECIFIC POKEMON
DELETE FROM pokemon 
WHERE pokemon.id=[id];

# DELETE A POKEMON *** THIS CASCADES THROUGH INDIVIDUAL POKEMON
DELETE FROM pokedex
WHERE pokedex.id = [id];

# CHANGE POKEMON OWNERSHIP ???
# There must be a better way to do this::
SET AUTOCOMMIT=0;
REPLACE INTO pokemon (id, poke_id, player_id)
VALUES (8, 2, 1);
COMMIT;

# SHOW ALL REGISTERED POKEMON AND THEIR TRAINERS, NAMES ONLY
SELECT pokedex.name, player.name AS "Trainer"
FROM pokemon 
INNER JOIN pokedex ON poke_id = pokedex.id
INNER JOIN player ON player_id = player.id;

# SHOW ALL OF GARY'S POKEMON THAT ARE WEAK AGAINST PIKACHU
SELECT pokedex.name, player.name AS "Trainer"
FROM pokemon 
INNER JOIN pokedex ON poke_id = pokedex.id
INNER JOIN player ON player_id = player.id
WHERE player_id = 2
AND (type1 IN (
  SELECT d_type FROM type_relation
  WHERE relation = 1
  AND a_type = (
    SELECT type1 FROM pokedex
    WHERE name = "Pikachu"
    )
) OR type2 IN (
  SELECT d_type FROM type_relation
  WHERE relation = 1
  AND a_type = (
    SELECT type1 FROM pokedex
    WHERE name = "Pikachu"
    )
));

# CREATE FULL TABLE OF ALL STRENGTHS/WEAKNESSES OF PLAYER'S POKEMON
SELECT *
FROM pokemon
INNER JOIN pokedex ON poke_id = pokedex.id
INNER JOIN player ON player_id = player.id
RIGHT JOIN type_relation ON type_relation.a_type = type1
RIGHT JOIN type_relation tr ON tr.a_type = type2
WHERE player.id = 1
;

# CREATE A TABLE OF ALL FAVORABLE / UNFAVORABLE MATCHUPS WITH AN OPPONENT'S POKEMON
# This only works with one type. Queries starting to get REALLY COMPLICATED.
# This query should be the main feature of my database.
SELECT pokedex.name, type.name AS "main type", type_relation.relation, opponent_pokemon, opponent_type
FROM pokemon
INNER JOIN pokedex ON poke_id = pokedex.id
INNER JOIN player ON player_id = player.id
INNER JOIN type ON type.id = pokedex.type1
INNER JOIN type_relation ON type_relation.a_type = type1
LEFT JOIN (
  SELECT pokedex.name AS "opponent_pokemon", type1, type2, type.name AS "opponent_type"
  FROM pokemon
  INNER JOIN pokedex ON poke_id = pokedex.id
  INNER JOIN player ON player_id = player.id
  INNER JOIN type ON type.id = pokedex.type1
  WHERE player.id = 2
  ) AS opponent ON type_relation.d_type = opponent.type1
WHERE player.id = 1 AND opponent_pokemon IS NOT NULL
GROUP BY pokedex.name, opponent_pokemon;

#END
