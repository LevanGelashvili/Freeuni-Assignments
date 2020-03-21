from easyzone import easyzone
from os import listdir
from ParserHelpers import types

# Caches info locally in a hashmap, key of which is a (domain, record_type) and value is a list of possible records
class Cache :

    # This implementation assumes we have multiple zone files provided, will work for single one too
    # Path to zone files must be passed
    def __init__(self, path) :
        self.cache, self.zones = {}, list(map(lambda zone: easyzone.zone_from_file(zone.split('.conf')[0], path + zone), listdir(path)))

        # Iterate over every zone
        for zone in self.zones :
            
            # Extract its domains and possible values
            for domain, name_obj in zone.get_names().items() :

                # Check for every single record type
                for record_type in types.values() :

                    record = name_obj.records(record_type)

                    #If we have any record for given type, add it to the cache
                    if record is not None : Cache.checkAndAdd(self, (domain, record_type), record.get_items())

    
    # Checks for record, creates a new set if no record's present
    def checkAndAdd(self, key, value) :
        if key not in self.cache : self.cache[key] = []
        if value not in self.cache[key] : self.cache[key].append(value)