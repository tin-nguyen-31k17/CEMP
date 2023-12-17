import 'package:sqflite/sqflite.dart';
import 'package:path/path.dart';
import 'package:fl_chart/fl_chart.dart';

class DatabaseHelper {
  static const _databaseName = "SensorData.db";
  static const _databaseVersion = 2;
  static const table = 'sensor_data';
  static const columnId = '_id';
  static const columnTimestamp = 'timestamp';
  static const columnValue = 'value';
  static const columnSensorId = 'sensor_id';
  static const maxEntriesPerSensor = 10; // Max entries per sensor

  DatabaseHelper._privateConstructor();
  static final DatabaseHelper instance = DatabaseHelper._privateConstructor();

  static Database? _database;
  Future<Database> get database async => _database ??= await _initDatabase();

  _initDatabase() async {
    String path = join(await getDatabasesPath(), _databaseName);
    return await openDatabase(path,
        version: _databaseVersion, 
        onCreate: _onCreate,
        onUpgrade: _onUpgrade);
  }

  Future _onCreate(Database db, int version) async {
    await db.execute('''
      CREATE TABLE $table (
        $columnId INTEGER PRIMARY KEY,
        $columnTimestamp INTEGER NOT NULL,
        $columnValue REAL NOT NULL,
        $columnSensorId TEXT NOT NULL
      )
    ''');
  }

  Future _onUpgrade(Database db, int oldVersion, int newVersion) async {
    if (oldVersion < 2) {
      await db.execute('ALTER TABLE $table ADD COLUMN $columnSensorId TEXT NOT NULL DEFAULT ""');
    }
  }

  Future<void> insertSensorData(SensorData data) async {
    Database db = await instance.database;
    // Insert new data
    await db.insert('sensor_data', data.toMap(),
        conflictAlgorithm: ConflictAlgorithm.replace);

    // Delete old data to keep the last 10 records for each sensor
    await db.rawDelete('''
      DELETE FROM $table
      WHERE $columnId NOT IN (
        SELECT $columnId FROM $table
        WHERE $columnSensorId = ?
        ORDER BY $columnTimestamp DESC
        LIMIT 10
      ) AND $columnSensorId = ?
    ''', [data.sensorId, data.sensorId]);
  }

   Future<List<FlSpot>> getHistoricalData(String sensorId) async {
    Database db = await instance.database;
    var maps = await db.query(
      'sensor_data',
      columns: ['timestamp', 'value'],
      where: '$columnSensorId = ?',
      whereArgs: [sensorId],
      orderBy: '$columnTimestamp DESC',
      limit: 10,
    );

    return maps.map((map) {
      double timestamp = (map['timestamp'] as int).toDouble();
      double value = (map['value'] as num).toDouble();
      return FlSpot(timestamp, value);
    }).toList().reversed.toList();
  }

  Future<void> _deleteOldEntries(Database db, String sensorId) async {
    await db.execute('''
      DELETE FROM $table WHERE $columnId NOT IN (
        SELECT $columnId FROM $table WHERE $columnSensorId = ? ORDER BY $columnTimestamp DESC LIMIT $maxEntriesPerSensor
      ) AND $columnSensorId = ?
    ''', [sensorId, sensorId]);
  }
}

class SensorData {
  final DateTime timestamp;
  final double value;
  final String sensorId;

  SensorData({required this.timestamp, required this.value, required this.sensorId});

  Map<String, dynamic> toMap() {
    return {
      'timestamp': timestamp.millisecondsSinceEpoch,
      'value': value,
      'sensor_id': sensorId,
    };
  }
}
