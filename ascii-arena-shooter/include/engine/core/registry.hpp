namespace engine
{
    class Registry
    {
        public:
            Entity create()
            {
                // generate and entity ID
                return Entity{nextEntity_++}
            }

            void destroy(Entity entity){
                // implement later
            };

        private:
        std::uint32_t nextEntity_ = 1;
    };
}