#include "database_mr.h"
#include "config.h"
#include "util.h"

const Util::FeatureMatchingMethod DEFAULT_FEATURE_MATCHING_METHOD = Util::FeatureMatchingMethod::STD;
const std::string DEFAULT_ORIGINALS_DIR = "./originals";
const std::string DEFAULT_CACHE_DIR = "./cache";
const int DEFAULT_MAXIMUM_RETURNED_MATCHES = 25;
const double DEFAULT_MAXIMUM_FEATURE_MATCHING_DISTANCE = 0.5;

SQLite::Database Database::db(""); // TODO bit hack-y; exceptions not caught

int Database::create(const boost::filesystem::path &database_path) {
	try {
		db = SQLite::Database(database_path.string(), SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
	}
	catch (std::exception &e) {
		if (PRINT_DB_ERRORS) {
			std::cout << "SQLite exception: " << e.what() << std::endl;
		}
		return EXIT_FAILURE;
	}
	return 0;
}

int Database::open(const boost::filesystem::path &database_path) {
	try {
		db = SQLite::Database(database_path.string(), SQLite::OPEN_READWRITE);
	}
	catch (std::exception &e) {
		if (PRINT_DB_ERRORS) {
			std::cout << "SQLite exception: " << e.what() << std::endl;
		}
		return EXIT_FAILURE;
	}

	if (create_metadata_table_if_needed()) {
		DatabaseMetadata metadata = DatabaseMetadata();
		metadata.backend_version_major = VERSION_MAJOR;
		metadata.backend_version_minor = VERSION_MINOR;
		metadata.database_version_major = VERSION_MAJOR;
		metadata.database_version_minor = VERSION_MINOR;

		metadata.feature_matching_method = DEFAULT_FEATURE_MATCHING_METHOD;

		metadata.originals_dir = DEFAULT_ORIGINALS_DIR;
		metadata.cache_dir = DEFAULT_CACHE_DIR;

		metadata.maximum_returned_matches = DEFAULT_MAXIMUM_RETURNED_MATCHES;
		metadata.maximum_feature_matching_distance = DEFAULT_MAXIMUM_FEATURE_MATCHING_DISTANCE;

		metadata.weight_surface_area = 0.1;
		metadata.weight_compactness = 0.1;
		metadata.weight_volume = 0.1;
		metadata.weight_diameter = 0.1;
		metadata.weight_eccentricity = 0.1;
		metadata.weight_A3 = 0.1;
		metadata.weight_D1 = 0.1;
		metadata.weight_D2 = 0.1;
		metadata.weight_D3 = 0.1;
		metadata.weight_D4 = 0.1;

		update_metadata(metadata);
	}

	create_shapes_table_if_needed();

	return 0;
}

bool Database::create_metadata_table_if_needed() {
	if (db.tableExists("metadata")) {
		return false;
	}

	const std::string query = "CREATE TABLE 'metadata' ("
	                          "'index' INTEGER UNIQUE NOT NULL,"
	                          "'backend_version_major' INTEGER NOT NULL,"
	                          "'backend_version_minor' INTEGER NOT NULL,"
	                          "'database_version_major' INTEGER NOT NULL,"
	                          "'database_version_minor' INTEGER NOT NULL,"
	                          "'feature_matching_method' TEXT NOT NULL DEFAULT '"
	                          + Util::ToString(DEFAULT_FEATURE_MATCHING_METHOD) +
	                          "',"
	                          "'originals_dir' TEXT NOT NULL DEFAULT './originals',"
	                          "'cache_dir' TEXT NOT NULL DEFAULT './cache',"
	                          "'maximum_returned_matches' INTEGER NOT NULL DEFAULT 25,"
	                          "'maximum_feature_matching_distance' REAL NOT NULL DEFAULT 0.5,"
	                          "'weight_surface_area' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_compactness' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_volume' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_diameter' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_eccentricity' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_A3' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_D1' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_D2' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_D3' REAL NOT NULL DEFAULT 0.1,"
	                          "'weight_D4' REAL NOT NULL DEFAULT 0.1,"
	                          "PRIMARY KEY('index' AUTOINCREMENT));";

	SQLite::Transaction transaction(db);
	db.exec(query);
	transaction.commit();
	return true;
}

bool Database::create_shapes_table_if_needed() {
	if (db.tableExists("shapes")) {
		return false;
	}

	const std::string query = "CREATE TABLE 'shapes' ("
	                          "'index' INTEGER UNIQUE NOT NULL,"
	                          "'filename' TEXT NOT NULL,"
	                          "'surface_area' REAL NOT NULL,"
	                          "'compactness' REAL NOT NULL,"
	                          "'volume' REAL NOT NULL,"
	                          "'diameter' REAL NOT NULL,"
	                          "'eccentricity' REAL NOT NULL,"
	                          "'a3' TEXT NOT NULL,"
	                          "'d1' TEXT NOT NULL,"
	                          "'d2' TEXT NOT NULL,"
	                          "'d3' TEXT NOT NULL,"
	                          "'d4' TEXT NOT NULL,"
	                          "'surface_area_normalized' REAL NOT NULL,"
	                          "'compactness_normalized' REAL NOT NULL,"
	                          "'volume_normalized' REAL NOT NULL,"
	                          "'diameter_normalized' REAL NOT NULL,"
	                          "'eccentricity_normalized' REAL NOT NULL,"
	                          "PRIMARY KEY('index' AUTOINCREMENT));";

	SQLite::Transaction transaction(db);
	db.exec(query);
	transaction.commit();
	return true;
}

void Database::update_metadata(const DatabaseMetadata &metadata) {
	const std::string query = "INSERT OR REPLACE INTO 'metadata' ("
	                          "'index',"
	                          "'backend_version_major',"
	                          "'backend_version_minor',"
	                          "'database_version_major',"
	                          "'database_version_minor',"
	                          "'feature_matching_method',"
	                          "'originals_dir',"
	                          "'cache_dir',"
	                          "'maximum_returned_matches',"
	                          "'maximum_feature_matching_distance',"
	                          "'weight_surface_area',"
	                          "'weight_compactness',"
	                          "'weight_volume',"
	                          "'weight_diameter',"
	                          "'weight_eccentricity',"
	                          "'weight_A3',"
	                          "'weight_D1',"
	                          "'weight_D2',"
	                          "'weight_D3',"
	                          "'weight_D4'"
	                          ") VALUES ("
	                          "(SELECT 'index' FROM 'metadata' WHERE 'index' = 1),"
	                          + to_string(metadata.backend_version_major) + ","
	                          + to_string(metadata.backend_version_minor) + ","
	                          + to_string(metadata.database_version_major) + ","
	                          + to_string(metadata.database_version_minor) + ","
	                          + "'" + Util::ToString(metadata.feature_matching_method) + "','"
	                          + metadata.originals_dir + "','"
	                          + metadata.cache_dir + "','"
	                          + to_string(metadata.maximum_returned_matches) + "','"
	                          + to_string(metadata.maximum_feature_matching_distance) + "','"
	                          + to_string(metadata.weight_surface_area) + "','"
	                          + to_string(metadata.weight_compactness) + "','"
	                          + to_string(metadata.weight_volume) + "','"
	                          + to_string(metadata.weight_diameter) + "','"
	                          + to_string(metadata.weight_eccentricity) + "','"
	                          + to_string(metadata.weight_A3) + "','"
	                          + to_string(metadata.weight_D1) + "','"
	                          + to_string(metadata.weight_D2) + "','"
	                          + to_string(metadata.weight_D3) + "','"
	                          + to_string(metadata.weight_D4) + "');";

	SQLite::Transaction transaction(db);
	db.exec(query);
	transaction.commit();
}

int Database::add_shape(const DatabaseShape &shape) {
	std::string path = shape.filename;

	const std::string query = "INSERT OR REPLACE INTO `shapes` ("
	                          "`index`,"
	                          "`filename`,"
	                          "`surface_area`,"
	                          "`compactness`,"
	                          "`volume`,"
	                          "`diameter`,"
	                          "`eccentricity`,"
	                          "`a3`,"
	                          "`d1`,"
	                          "`d2`,"
	                          "`d3`,"
	                          "`d4`,"
	                          "`surface_area_normalized`,"
	                          "`compactness_normalized`,"
	                          "`volume_normalized`,"
	                          "`diameter_normalized`,"
	                          "`eccentricity_normalized`)"
	                          " VALUES "
	                          "("
	                          "(SELECT `index` FROM `shapes` WHERE `index` = " + to_string(shape.index) + "),"
	                          + "\"" + path + "\","
	                          + to_string(shape.surface_area) + ","
	                          + to_string(shape.compactness) + ","
	                          + to_string(shape.volume) + ","
	                          + to_string(shape.diameter) + ","
	                          + to_string(shape.eccentricity) + ","
	                          + "\"" + Util::serialize(shape.a3) + "\","
	                          + "\"" + Util::serialize(shape.d1) + "\","
	                          + "\"" + Util::serialize(shape.d2) + "\","
	                          + "\"" + Util::serialize(shape.d3) + "\","
	                          + "\"" + Util::serialize(shape.d4) + "\","
	                          + to_string(shape.surface_area_normalized) + ","
	                          + to_string(shape.compactness_normalized) + ","
	                          + to_string(shape.volume_normalized) + ","
	                          + to_string(shape.diameter_normalized) + ","
	                          + to_string(shape.eccentricity_normalized) + ");";

	SQLite::Transaction transaction(db);
	db.exec(query);
	transaction.commit();
	return 0;
}

bool Database::add_shapes(const std::vector<DatabaseShape> &shapes) {
	bool result = true;
	for (const DatabaseShape &shape : shapes) {
		if (add_shape(shape) != 0) {
			result = false;
		}
	}
	return result;
}

DatabaseMetadata Database::metadata() {
	const std::string query = "SELECT * FROM `metadata` WHERE `index` = 1;";
	SQLite::Statement statement(db, query);

	DatabaseMetadata metadata = DatabaseMetadata();

	while (statement.executeStep()) {
		metadata.backend_version_major = statement.getColumn(1);
		metadata.backend_version_minor = statement.getColumn(2);
		metadata.database_version_major = statement.getColumn(3);
		metadata.database_version_minor = statement.getColumn(4);

		char feature_matching_method[256];
		strcpy(feature_matching_method, statement.getColumn(5));
		metadata.feature_matching_method = Util::FromString(std::string(feature_matching_method));

		char originals_dir[256];
		strcpy(originals_dir, statement.getColumn(6));
		metadata.originals_dir = originals_dir;

		char cache_dir[256];
		strcpy(cache_dir, statement.getColumn(7));
		metadata.cache_dir = cache_dir;

		metadata.maximum_returned_matches = statement.getColumn(8);
		metadata.maximum_feature_matching_distance = statement.getColumn(9);

		metadata.weight_surface_area = statement.getColumn(10);
		metadata.weight_compactness = statement.getColumn(11);
		metadata.weight_volume = statement.getColumn(12);
		metadata.weight_diameter = statement.getColumn(13);
		metadata.weight_eccentricity = statement.getColumn(14);
		metadata.weight_A3 = statement.getColumn(15);
		metadata.weight_D1 = statement.getColumn(16);
		metadata.weight_D2 = statement.getColumn(17);
		metadata.weight_D3 = statement.getColumn(18);
		metadata.weight_D4 = statement.getColumn(19);
	}

	return metadata;
}

vector<DatabaseShape> Database::shapes() {
	const std::string query = "SELECT * FROM `shapes`;";
	SQLite::Statement statement(db, query);

	vector<DatabaseShape> shapes = {};

	while (statement.executeStep()) {
		DatabaseShape shape = DatabaseShape();

		shape.index = statement.getColumn(0);

		char path_buffer[256];
		strcpy(path_buffer, statement.getColumn(1));
		shape.filename = path_buffer;

		shape.surface_area = statement.getColumn(2);
		shape.compactness = statement.getColumn(3);
		shape.volume = statement.getColumn(4);
		shape.diameter = statement.getColumn(5);
		shape.eccentricity = statement.getColumn(6);
		shape.a3 = Util::deserialize(statement.getColumn(7));
		shape.d1 = Util::deserialize(statement.getColumn(8));
		shape.d2 = Util::deserialize(statement.getColumn(9));
		shape.d3 = Util::deserialize(statement.getColumn(10));
		shape.d4 = Util::deserialize(statement.getColumn(11));

		shape.surface_area_normalized = statement.getColumn(12);
		shape.compactness_normalized = statement.getColumn(13);
		shape.volume_normalized = statement.getColumn(14);
		shape.diameter_normalized = statement.getColumn(15);
		shape.eccentricity_normalized = statement.getColumn(16);

		shapes.push_back(shape);
	}

	return shapes;
}

DatabaseShape Database::get_shape_from_filename(const std::string &filename) {
	std::vector<DatabaseShape> shapes = Database::shapes();

	DatabaseShape databaseShape;

	for (const DatabaseShape &shape : shapes) {
		if (shape.filename == filename) {
			databaseShape = shape;
			break;
		}
	}

	return databaseShape;
}

int Database::close() {
	db = SQLite::Database("");
	return 0;
}