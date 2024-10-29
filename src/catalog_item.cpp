#include "catalog_item.h"

CatalogItem::CatalogItem(const std::string& name, double price) : name(name), price(price) {
    dealId = -1;
}