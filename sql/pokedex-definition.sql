-- Travis J. Sanders
-- CS 275 Su14
-- 2014.08.06
-- Final Project

SET FOREIGN_KEY_CHECKS=0;

-- POKEDEX: Index of Names, Numbers and Types of Pokemon
-- Only one 'type' is required
DROP TABLE IF EXISTS `pokedex`;
CREATE TABLE pokedex (
  id SMALLINT NOT NULL AUTO_INCREMENT,
  name VARCHAR(255) NOT NULL UNIQUE,
  type1 TINYINT NOT NULL,
  type2 TINYINT DEFAULT NULL,
  CONSTRAINT fk_t1 FOREIGN KEY (type1) REFERENCES type (id) ON DELETE RESTRICT,
  CONSTRAINT fk_t2 FOREIGN KEY (type2) REFERENCES type (id) ON DELETE RESTRICT,
  PRIMARY KEY  (id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- TYPES: Fire, water, leaf, etc
DROP TABLE IF EXISTS `type`;
CREATE TABLE type (
	id TINYINT NOT NULL AUTO_INCREMENT,
	name VARCHAR(255) NOT NULL UNIQUE,
	PRIMARY KEY (id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- MANY-TO-MANY TABLE: Types relative to other types
-- attack / defnese
-- 1 = SUPER EFFECTIVE; -1 Not very effective; -2 No effect; NULL/no entry = normal
DROP TABLE IF EXISTS `type_relation`;
CREATE TABLE type_relation (
	-- id INT NOT NULL AUTO_INCREMENT,
	a_type TINYINT NOT NULL,
	d_type TINYINT NOT NULL,
	relation SMALLINT DEFAULT NULL,
	CONSTRAINT fk_a FOREIGN KEY (a_type) REFERENCES type (id) ON DELETE CASCADE,
	CONSTRAINT fk_d FOREIGN KEY (d_type) REFERENCES type (id) ON DELETE CASCADE,
	PRIMARY KEY (a_type, d_type)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- SPECIFIC POKEMON: nicknames and levels
DROP TABLE IF EXISTS `pokemon`;
CREATE TABLE pokemon (
  id INT NOT NULL AUTO_INCREMENT,
  poke_id SMALLINT NOT NULL,
  nickname VARCHAR(255),
  level TINYINT NOT NULL DEFAULT 5,
  player_id INT DEFAULT NULL,
  CONSTRAINT fk_pokeid FOREIGN KEY (poke_id) REFERENCES pokedex (id) ON DELETE CASCADE,
  CONSTRAINT fk_playid FOREIGN KEY (player_id) REFERENCES player (id) ON DELETE CASCADE,
  PRIMARY KEY  (id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- PLAYERS: (User accounts)
DROP TABLE IF EXISTS `player`;
CREATE TABLE player (
	id INT NOT NULL AUTO_INCREMENT,
	name VARCHAR(255) NOT NULL UNIQUE,
	PRIMARY KEY (id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- Fill table with all original Pokemon types
-- (No 'fairy' or 'steel' types. These were added later.)
SET AUTOCOMMIT=0;
INSERT INTO type (id, name) 
VALUES 
(1, "normal"), 
(2, "fire"), 
(3, "water"), 
(4, "electric"), 
(5, "grass"), 
(6, "ice"), 
(7, "fighting"), 
(8, "poison"), 
(9, "ground"), 
(10, "flying"), 
(11, "psychic"),
(12, "bug"), 
(13, "rock"), 
(14, "ghost"), 
(15, "dragon");
COMMIT;

-- Fill table with all original 151 Pokemon
SET AUTOCOMMIT=0;
INSERT INTO pokedex (id, name, type1, type2) 
VALUES 
(001, "Bulbasaur", 5, 8),
(002, "Ivysaur", 5, 8),
(003, "Venusaur", 5, 8),
(004, "Charmander", 2, NULL),
(005, "Charmeleon", 2, NULL),
(006, "Charizard", 2, 10),
(007, "Squirtle", 3, NULL),
(008, "Wartortle", 3, NULL),
(009, "Blastoise", 3, NULL),
(010, "Caterpie", 12, NULL),
(011, "Metapod", 12, NULL),
(012, "Butterfree", 12, 10),
(013, "Weedle", 12, NULL),
(014, "Kakuna", 12, NULL),
(015, "Beedrill", 12, NULL),
(016, "Pidgey", 1, 10),
(017, "Pidgeotto", 1, 10),
(018, "Pidgeot", 1, 10),
(019, "Rattata", 1, NULL),
(020, "Raticate", 1, NULL),
(021, "Spearow", 1, 10),
(022, "Fearow", 1, 10),
(023, "Ekans", 8, NULL),
(024, "Arbok", 8, NULL),
(025, "Pikachu", 4, NULL),
(026, "Raichu", 4, NULL),
(027, "Sandshrew", 9, NULL),
(028, "Sandslash", 9, NULL),
(029, "Nidoran♀", 8, NULL),
(030, "Nidorina", 8, NULL),
(031, "Nidoqueen", 8, 9),
(032, "Nidoran♂", 8, NULL),
(033, "Nidorino", 8, NULL),
(034, "Nidoking", 8, 9),
(035, "Clefairy", 1, NULL),
(036, "Clefable", 1, NULL),
(037, "Vulpix", 2, NULL),
(038, "Ninetales", 2, NULL),
(039, "Jigglypuff", 1, NULL),
(040, "Wigglytuff", 1, NULL),
(041, "Zubat", 8, 10),
(042, "Golbat", 8, 10),
(043, "Oddish", 5, 8),
(044, "Gloom", 5, 8),
(045, "Vileplume", 5, 8),
(046, "Paras", 12, 5),
(047, "Parasect", 12, 5),
(048, "Venonat", 12, 8),
(049, "Venomoth", 12, 8),
(050, "Diglett", 9, NULL),
(051, "Dugtrio", 9, NULL),
(052, "Meowth", 1, NULL),
(053, "Persian", 1, NULL),
(054, "Psyduck", 3, NULL),
(055, "Golduck", 3, NULL),
(056, "Mankey", 7, NULL),
(057, "Primeape", 7, NULL),
(058, "Growlithe", 2, NULL),
(059, "Arcanine", 2, NULL),
(060, "Poliwag", 3, NULL),
(061, "Poliwhirl", 3, NULL),
(062, "Poliwrath", 3, 7),
(063, "Abra", 11, NULL),
(064, "Kadabra", 11, NULL),
(065, "Alakazam", 11, NULL),
(066, "Machop", 7, NULL),
(067, "Machoke", 7, NULL),
(068, "Machamp", 7, NULL),
(069, "Bellsprout", 5, 8),
(070, "Weepinbell", 5, 8),
(071, "Victreebel", 5, 8),
(072, "Tentacool", 3, 8),
(073, "Tentacruel", 3, 8),
(074, "Geodude", 13, 9),
(075, "Graveler", 13, 9),
(076, "Golem", 13, 9),
(077, "Ponyta", 2, NULL),
(078, "Rapidash", 2, NULL),
(079, "Slowpoke", 3, 11),
(080, "Slowbro", 3, 11),
(081, "Magnemite", 4, NULL),
(082, "Magneton", 4, NULL),
(083, "Farfetch'd", 1, 10),
(084, "Doduo", 1, 10),
(085, "Dodrio", 1, 10),
(086, "Seel", 3, NULL),
(087, "Dewgong", 3, 6),
(088, "Grimer", 8, NULL),
(089, "Muk", 8, NULL),
(090, "Shellder", 3, NULL),
(091, "Cloyster", 3, 6),
(092, "Gastly", 14, 8),
(093, "Haunter", 14, 8),
(094, "Gengar", 14, 8),
(095, "Onix", 13, NULL),
(096, "Drowzee", 11, NULL),
(097, "Hypno", 11, NULL),
(098, "Krabby", 3, NULL),
(099, "Kingler", 3, NULL),
(100, "Voltorb", 4, NULL),
(101, "Electrode", 4, NULL),
(102, "Exeggcute", 5, 11),
(103, "Exeggutor", 5, 11),
(104, "Cubone", 9, NULL),
(105, "Marowak", 9, NULL),
(106, "Hitmonlee", 7, NULL),
(107, "Hitmonchan", 7, NULL),
(108, "Lickitung", 1, NULL),
(109, "Koffing", 8, NULL),
(110, "Weezing", 8, NULL),
(111, "Rhyhorn", 9, 13),
(112, "Rhydon", 9, 13),
(113, "Chansey", 1, NULL),
(114, "Tangela", 5, NULL),
(115, "Kangaskhan", 1, NULL),
(116, "Horsea", 3, NULL),
(117, "Seadra", 3, NULL),
(118, "Goldeen", 3, NULL),
(119, "Seaking", 3, NULL),
(120, "Staryu", 3, NULL),
(121, "Starmie", 3, 11),
(122, "Mr. Mime", 11, NULL),
(123, "Scyther", 1, 10),
(124, "Jynx", 6, 11),
(125, "Electabuzz", 4, NULL),
(126, "Magmar", 2, NULL),
(127, "Pinsir", 12, NULL),
(128, "Tauros", 1, NULL),
(129, "Magikarp", 3, NULL),
(130, "Gyarados", 3, 10),
(131, "Lapras", 3, 6),
(132, "Ditto", 1, NULL),
(133, "Eevee", 1, NULL),
(134, "Vaporeon", 3, NULL),
(135, "Jolteon", 4, NULL),
(136, "Flareon", 2, NULL),
(137, "Porygon", 1, NULL),
(138, "Omanyte", 13, 3),
(139, "Omastar", 13, 3),
(140, "Kabuto", 13, 3),
(141, "Kabutops", 13, 3),
(142, "Aerodactyl", 13, 10),
(143, "Snorlax", 1, NULL),
(144, "Articuno", 6, 10),
(145, "Zapdos", 4, 10),
(146, "Moltres", 2, 10),
(147, "Dratini", 15, NULL),
(148, "Dragonair", 15, NULL),
(149, "Dragonite", 15, 10),
(150, "Mewtwo", 11, NULL),
(151, "Mew", 11, NULL);
COMMIT;

-- Fill table of all of the "type" relationships
SET AUTOCOMMIT=0;
INSERT INTO type_relation (a_type, d_type, relation)
VALUES 
(1,13,-1), (1,14,-2),
(2,2,-1), (2,3,-1), (2,5,1), (2,6,1), (2,12,1), (2,13,-1), (2,15,-1), 
(3,2,1), (3,3,-1), (3,5,-1), (3,9,1), (3,13,1), (3,15,-1), 
(4,3,1), (4,4,-1), (4,5,-1), (4,9,-2), (4,10,1), (4,15,-1), 
(5,2,-1), (5,3,1), (5,5,-1), (5,8,-1), (5,9,1), (5,10,-1), (5,12,-1), (5,13,1), (5,15,-1), 
(6,3,-1), (6,5,1), (6,6,-1), (6,10,1), (6,15,1), 
(7,1,1), (7,6,1), (7,8,-1), (7,10,-1), (7,11,-1), (7,12,-1), (7,13,1), (7,14,-2), 
(8,5,1), (8,8,-1), (8,9,-1), (8,12,1), (8,13,-1), (8,14,-1), 
(9,2,1), (9,4,1), (9,5,-1), (9,8,1), (9,10,-2), (9,12,-1), (9,13,1), 
(10,4,-1), (10,5,1), (10,7,1), (10,12,1), (10,13,-1), 
(11,7,1), (11,8,1), (11,11,-1),
(12,2,-1), (12,5,1), (12,7,-1), (12,8,1), (12,10,-1), (12,11,1), (12,14,-1), 
(13,2,1), (13,6,1), (13,7,-1), (13,9,-1), (13,10,1), (13,12,1),
(14,14,1),
(15,15,1);
COMMIT;

-- Create two players
SET AUTOCOMMIT=0;
INSERT INTO player (name) VALUES ("Ash"), ("Gary");
COMMIT;

-- Create 6 Pokemon for each player at the default level of 5
SET AUTOCOMMIT=0;
INSERT INTO pokemon (poke_id, player_id)
VALUES
(145, 1),(1, 1),(25, 1),(4, 1),(7, 1),(9, 1),
(144, 2),(2, 2),(26, 2),(3, 2),(6, 2),(51, 2);
COMMIT;
